#include "particle_system.hpp"

void ParticleSystem::update(float dt) {
    for (int step = 0; step < subSteps; step++){
        applyGravity();
        updatePositions(dt / subSteps);
        applyConstraints();
        resolveCollisions();
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

void ParticleSystem::resolveCollisions() {
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