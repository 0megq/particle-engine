#ifndef PARTICLE_H
#define PARTICLE_H
#include "raylib-cpp.hpp"

// Header-only struct
struct Particle {
    float radius = 1.0f;
    raylib::Color color;
    raylib::Vector3 posCur;
    raylib::Vector3 posOld;
    raylib::Vector3 acc = {};

    // Update position using Verlet integration with internal fields
    // `pos_cur`, `pos_old`, and `acc`. `dt` is the time step (in seconds).
    void updatePosition(float dt) {
        auto posNew = posCur * 2.0f - posOld + acc * (dt * dt) * 0.5f;
        posOld = posCur;
        posCur = posNew;
        acc = {};
    }

    // circle to circle intersection test. If tolerance is positive then the combined
    // radius is larger and a intersection is detected before particles are actually
    // colliding.
    bool intersects(Particle other, float radiusTolerance = -0.001f) {
        auto const combinedRadiusWithTolerance = radius + other.radius + radiusTolerance;
        auto const squaredDistance = (posCur - other.posCur).LengthSqr();
        return squaredDistance < combinedRadiusWithTolerance * combinedRadiusWithTolerance;
    }
};


#endif