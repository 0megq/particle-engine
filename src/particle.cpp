#include "particle.hpp"


void Particle::updatePosition(float dt) {
    auto posNew = posCur * 2.0f - posOld + acc * (dt * dt) * 0.5f;
    posOld = posCur;
    posCur = posNew;
    acc = {};
}