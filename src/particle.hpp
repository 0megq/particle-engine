#ifndef PARTICLE_H
#define PARTICLE_H
#include "raylib-cpp.hpp"

struct Particle {
    float radius = 1.0f;
    raylib::Color color;
    raylib::Vector3 posCur;
    raylib::Vector3 posOld;
    raylib::Vector3 acc = {};

    // Update position using Verlet integration with internal fields
    // `pos_cur`, `pos_old`, and `acc`. `dt` is the time step (in seconds).
    void updatePosition(float dt);
};


#endif