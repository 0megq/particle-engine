#ifndef OCTREE_H
#define OCTREE_H

#include <array>
#include <vector>
#include <memory>
#include "raylib-cpp.hpp"
#include "particle.hpp"
#include "aabb.hpp"

class Octree {
    private:
        /// max amount of particles before we subdivide
        static constexpr std::size_t capacity = 4;
        static constexpr std::size_t maxDepth = 5;

        /// coordinates of children are identified using 3-bit strong
        /// bits correspond to xyz and 0 -> -1, and 1 -> 1. So 100 represents
        /// (1,-1,-1)
        std::array<std::unique_ptr<Octree>, 8> m_children;

        /// Particles can be stored as direct raw pointers since the octree
        /// is reconstructed every update just for the collision check. We don't
        /// need to worry about reallocation safety.
        std::vector<Particle*> m_particles;
        std::size_t m_depth;
        AABB m_boundary;

        static raylib::Vector3 indexToVec(std::size_t idx);

        
        /// Converts vec into an index value to be used with
        /// m_children. Bitwise operation is used to set the first
        /// bit to 1 if x > 0, and 0 if x < 0. 
        /// 
        /// If vec has a component with 0, -1 is returned.
        /// -1 is used instead of wrapping with std::optional or
        /// using std::numeric_limits<size_t>::max() for the sake
        /// of simplicity.
        static int vecToIndex(raylib::Vector3 vec);

        AABB getChildBoundary(std::size_t idx) const;
        /// Returns 0-7 based on which child octant the given AABB
        /// fits into. If it is not entirely contained by a single octant
        /// then -1 is returned. The given region is expected to already
        /// fit inside this Octree's boundary, since outer bounds are not
        /// checked
        int getOctant(const AABB &region) const;
        void subdivide();
        bool isLeaf() const;
        void collectIntersectionsInDescendants(Particle * particle, std::vector<std::pair<Particle*,Particle*>> &intersections) const;
        void findAllIntersections(std::vector<std::pair<Particle*,Particle*>> &intersections) const;

    public:
        // Constructs the entire octree, deriving the boundary
        // so that it contains all particles, and then inserting
        // all particles into the tree. O(n * log(n))
        Octree(std::vector<Particle> &particles);
        Octree(const AABB &boundary, std::size_t depth);
        
        /// The particle's AABB must fit within the m_boundary. O(log(n))
        void insert(Particle *);
        std::vector<std::pair<Particle *, Particle *>> findAllIntersections() const;
        
};


#endif