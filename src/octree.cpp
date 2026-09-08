#include <octree.hpp>
#include <cassert>

raylib::Vector3 Octree::indexToVec(std::size_t idx) {
    // I chose to use bitwise operators instead of
    // modulo by 2, because bitwise is truer to what
    // the idx actually translates to.
    float x = (idx & 0b100) ? 1.0 : -1.0;
    float y = (idx & 0b010) ? 1.0 : -1.0;
    float z = (idx & 0b001) ? 1.0 : -1.0;

    return raylib::Vector3{x, y, z};
}

int Octree::vecToIndex(raylib::Vector3 vec) {
    if (vec.x == 0 || vec.y == 0 || vec.z) return -1;

    int idx = 0b000;
    if (vec.x > 0) idx |= 0b100;
    if (vec.y > 0) idx |= 0b010;
    if (vec.z > 0) idx |= 0b001;

    return idx;
}

AABB Octree::getChildBoundary(std::size_t idx) const {
    auto offset = indexToVec(idx) * 0.5 * m_boundary.halfSize;

    return AABB{m_boundary.center + offset, m_boundary.halfSize * 0.5};
}

int Octree::getOctant(const AABB &region) const {
    auto relativeCenter = region.center - m_boundary.center;

    // If the region crosses one axes, then it intersects multiple child octants
    if (abs(relativeCenter.x) < region.halfSize.x || 
        abs(relativeCenter.y) < region.halfSize.y || 
        abs(relativeCenter.z) < region.halfSize.z)
        return -1;

    return vecToIndex(relativeCenter);
}

void Octree::collectIntersectionsInDescendants(Particle *particle, std::vector<std::pair<Particle*,Particle*>> &intersections) const {
    for (auto other : m_particles) {
        intersections.emplace_back(particle, other);
    }
 
    if (!isLeaf()) {
        // collect intersections from children
        for (std::size_t i = 0; i < 8; i++) {
            m_children[i]->collectIntersectionsInDescendants(particle, intersections);
        }
    }
}

void Octree::findAllIntersections(std::vector<std::pair<Particle*,Particle*>> &intersections) const {
    // for each particle
    for (int i = 0; i < m_particles.size(); i++) {
        for (int j = 0; j < i; j++) {
            intersections.emplace_back(m_particles[i], m_particles[j]);
        }
    }

    if (!isLeaf()) {
        // for each particle check intersection with child particles findIntersectionsWithDescendants
        for (auto particle : m_particles) {
            for (std::size_t i = 0; i < 8; i++) {
                m_children[i]->collectIntersectionsInDescendants(particle, intersections);
            }
        }

        // collect intersections from children
        for (std::size_t i = 0; i < 8; i++) {
            m_children[i]->findAllIntersections(intersections);
        }
    }
}

void Octree::subdivide() {
    // initialize children
    for (std::size_t i = 0; i < 8; i++) {
        m_children[i] = std::make_unique<Octree>(getChildBoundary(i), m_depth + 1);
    }

    // Try to insert particles into children;
    // save ones that don't fit for this node
    std::vector<Particle *> newParticles;
    newParticles.reserve(capacity);
    
    for (Particle *particle : m_particles) {
        const AABB particleAABB = AABB{particle->posCur, raylib::Vector3::One() * particle->radius};
        auto octant = getOctant(particleAABB);

        // If it doesn't fit into a single child
        if (octant == -1) {
            // Just put it back
            newParticles.push_back(particle);
            continue;
        }

        // Otherwise, add it to child
        m_children[static_cast<std::size_t>(octant)]->insert(particle);
    }

    // I use std::move instead of a raw `= newParticles`, so that
    // m_particles directly steals the heap memory, instead of 
    // allocating new memory and copying 1-by-1. O(1) vs O(n)
    m_particles = std::move(newParticles);
}

bool Octree::isLeaf() const {
    return m_children[0].get() == nullptr;
}

Octree::Octree(std::vector<Particle> &particles) : m_depth(0) {
    m_particles.resize(capacity);

    // Get boundary
    bool first = true;
    raylib::Vector3 min;
    raylib::Vector3 max;

    // Set boundary
    for (const auto &particle : particles) {
        if (first) {
            min = particle.posCur;
            max = particle.posCur;
            first = false;
            continue;
        }

        min = min.Min(particle.posCur);
        max = max.Min(particle.posCur);
    }

    m_boundary.halfSize = (max - min) * 0.5;
    m_boundary.center = (max + min) * 0.5;

    // Insert all particles
    for (auto &particle : particles) {
        insert(&particle);
    }
}


Octree::Octree(const AABB &boundary, std::size_t depth) : m_boundary(boundary), m_depth(depth) {
    m_particles.resize(capacity);
}

void Octree::insert(Particle *particle) {
    const AABB particleAABB = AABB{particle->posCur, raylib::Vector3::One() * particle->radius};

    // Particle is outside the bounds, so don't insert
    assert(m_boundary.intersects(particleAABB) && "Octree: inserting particle that is not within boundary!");

    if (isLeaf()) {
        // If not at capacity or at max depth, then put it here
        if (m_particles.size() < capacity || m_depth >= maxDepth) {
            m_particles.push_back(particle);
            return;
        }

        // split and try again
        subdivide();
        insert(particle);
        return;
    }

    auto octant = getOctant(particleAABB);
    // if -1, then no child fully contains this particle
    if (octant == -1) {
        // so, stop recursion and place here
        m_particles.push_back(particle);
        return;
    }

    m_children[static_cast<std::size_t>(octant)]->insert(particle);
}

std::vector<std::pair<Particle *, Particle *>> Octree::findAllIntersections() const {
    std::vector<std::pair<Particle *, Particle *>> res;
    findAllIntersections(res);
    return res;
}