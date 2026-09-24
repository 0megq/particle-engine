#include "particle_system.hpp"
#include <cstring>
#include <algorithm>
#include <map>

void ParticleSystem::update(float dt) {
    for (int step = 0; step < subSteps; step++){
        applyGravity();
        updatePositions(dt / subSteps);
        resolveCollisionsN2();
        applyConstraints();
        
    }
}

void ParticleSystem::applyGravity() {
    for (Particle &p : m_particles) {
        p.acc += {0.0f, -32.0f, 0.0f};
    }
}

void ParticleSystem::updatePositions(float dt) {
    for (Particle &p : m_particles) {
        p.updatePosition(dt);
    }
}

void ParticleSystem::applyConstraints() {
    for (Particle &p : m_particles) {
        auto const combinedRadius = (boundaryRadius - p.radius);
        if (p.posCur.LengthSqr() > combinedRadius * combinedRadius) {
            p.posCur = p.posCur.Normalize() * combinedRadius;
        }
    }
}

void ParticleSystem::resolveCollisionsOctree() {
    // insert quadtree heres
    Octree tree = Octree(m_particles);

    for (auto &pair : tree.findAllIntersections()) {
        auto p = pair.first;
        auto o = pair.second;
        auto const combinedRadius = (o->radius + p->radius);
        auto const combinedRadiusWithTolerance = combinedRadius - collisionTolerance;
        auto const pToO = o->posCur - p->posCur;
        if (pToO.LengthSqr() < combinedRadiusWithTolerance * combinedRadiusWithTolerance) {
            auto const depth = combinedRadius - pToO.Length();
            auto const normal = pToO.Normalize();
            p->posCur += -normal * depth * 0.5f;
            o->posCur += normal * depth * 0.5f;
        }
    }
}

void ParticleSystem::resolveCollisionsN2() {
    for (Particle &p : m_particles) {
        for (Particle &o : m_particles) {
            auto const combinedRadius = (o.radius + p.radius);
            auto const combinedRadiusWithTolerance = combinedRadius - collisionTolerance;
            auto const pToO = o.posCur - p.posCur;
            if (pToO.LengthSqr() < combinedRadiusWithTolerance * combinedRadiusWithTolerance) {
                auto const depth = combinedRadius - pToO.Length();
                auto const normal = pToO.Normalize();
                p.posCur += -normal * depth * 0.5f;
                o.posCur += normal * depth * 0.5f;
            }
        }
    }
}

void ParticleSystem::resolveCollisionsSAP() {
    // intervals will need to have reference to their object as a pointer or stable idx (handler would be cool)
    // and interval needs isLeft flag. the actual position will just be grabbed from the actual object
    // this will be useful later when i want to implement insertion sort and keep the list across frames
    // we will need to do extra checks to make sure the indices or pointers remain valid

    // get sorted intervals in X and add any new particles
    if (m_edgesX.empty()) {
        // add all intervals - O(N)
        for (size_t idx = 0; idx < m_particles.size(); idx++) {
            m_edgesX.push_back(
                SAPEdge {
                    m_particles[idx].posCur.x - m_particles[idx].radius,
                    idx,
                    true,
                }
            );
            m_edgesX.push_back(
                SAPEdge {
                    m_particles[idx].posCur.x + m_particles[idx].radius,
                    idx,
                    false,                    
                }
            );
        }

        // sort intervals - O(Nlog(N))
        std::sort(m_edgesX.front(), m_edgesX.back(), [](SAPEdge *left, SAPEdge *right) -> bool {return left->pos < right->pos;});
    }


    // loop through intervals, add object to active list on start interval, remove on end interval
        // build a list of pairs in a hashset so accessing is quick

    // repeat for each 
    
    // narrow phase on the pairs

    // after this point sorting of the list cannot be guaranteed because the particle may change positions
    // for now i just clear the list
    m_edgesX.clear();
}

void ParticleSystem::render() {
    for (Particle &p : m_particles) {
        DrawSphere(p.posCur, p.radius, p.color);
    }
    DrawSphereWires({}, boundaryRadius, 8, 16, {255, 255, 255, 255});
    DrawSphere({}, boundaryRadius, {255, 255, 255, 30});
}

void ParticleSystem::addParticle(raylib::Vector3 pos, raylib::Vector3 vel, float delta, raylib::Color color, float radius) {
    m_particles.push_back(Particle{
        radius,
        color,
        pos,
        pos - vel * delta,
        0
    });
}
