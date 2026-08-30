#include "particle.hpp"

Particle::Particle(raylib::Vector3 pos) {
    m_pos = pos;
    m_radius = 1.0f;
}

void Particle::draw() {
    DrawSphere(m_pos, m_radius, raylib::Color::Red());
}