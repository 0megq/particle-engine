#ifndef PARTICLE_H
#define PARTICLE_H
#include "raylib-cpp.hpp"

struct Particle {
    float m_radius;
    raylib::Vector3 m_pos;

    Particle(raylib::Vector3);
    void draw();
};


#endif