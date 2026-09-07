#ifndef AABB_H
#define AABB_H

#include "raylib-cpp.hpp"
#include "particle.hpp"

/**
 * Simple AABB struct storing center and halfSize
 * 
 * I also chose to store center and halfSize instead of min and max, because
 * this AABB will be used for an Octree. center + halfSize is more straightforward
 * for splitting regions in the octree.
 * 
 * Note: This is a really simple struct so I've opted to keep it header only.
 **/ 
struct AABB {
    raylib::Vector3 center;
    raylib::Vector3 halfSize;

    bool intersects(const AABB &other) {
        return (abs(center.x - other.center.x) < halfSize.x + other.halfSize.x) &&
                (abs(center.y - other.center.y) < halfSize.y + other.halfSize.y) &&
                (abs(center.z - other.center.z) < halfSize.z + other.halfSize.z);
    }
};

#endif