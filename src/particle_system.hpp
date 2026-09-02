#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H
#include <vector>
#include "raylib-cpp.hpp"
#include "particle.hpp"

class ParticleSystem {
    private:
        static constexpr float collisionTolerance = 0.01f;
        static constexpr float boundaryRadius = 20.0f;
        static constexpr int subSteps = 2;

        std::vector<Particle> m_particles;
        void updatePositions(float dt);
        void applyGravity();
        void applyConstraints();
        void resolveCollisions();

    public:
        // Must be called with a fixed dt
        void update(float dt);
        void render();
        void addParticle(raylib::Vector3 pos, raylib::Vector3 vel, float delta, raylib::Color color, float radius = 0.2f);
};

#endif