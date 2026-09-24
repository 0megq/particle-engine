#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H
#include <vector>
#include "raylib-cpp.hpp"
#include "particle.hpp"
#include "octree.hpp"

class ParticleSystem {
    private:
        struct SAPEdge {
            float pos;
            size_t particleIdx;
            bool isLeft;
        };

        static constexpr float collisionTolerance = 0.01f;
        static constexpr float boundaryRadius = 20.0f;
        static constexpr int subSteps = 2;

        std::vector<SAPEdge> m_edgesX;
        std::vector<Particle> m_particles;
        void updatePositions(float dt);
        void applyGravity();
        void applyConstraints();
        
        void resolveCollisionsOctree();
        void resolveCollisionsN2();
        void resolveCollisionsSAP();

        
    public:
        // Must be called with a fixed dt
        void update(float dt);
        void render();
        void addParticle(raylib::Vector3 pos, raylib::Vector3 vel, float delta, raylib::Color color, float radius);
};

#endif