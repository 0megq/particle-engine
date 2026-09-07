# Particle engine in C++

Uses raylib library. CMakeLists.txt is a modified version from [raylib-cpp's cmake template](https://github.com/RobLoach/raylib-cpp/blob/master/projects/CMake/CMakeLists.txt)


# interesting things to highlight:
- verlet integration
- substepping

# planned
- optimize collisions
  - broadphase (octree)
    - construct root node with bounds set to max and min positions of all particles, but use a single halfSize for all axes since particles have same radius for all axes
    - dynamically find the max depth, based on total bounds and average (or median or mode) particle radius
    - reason about the time complexity of having a particle in multiple child trees, when it overlaps multiple. OR have it just once in the parent
    - using indices instead of pointers
  - sleeping still particles
  - sweep and prune with insertion sort (insertion sort because of temporal coherence)
    - optimized SAP method: https://ieeexplore.ieee.org/document/10121435
- moving around with camera
- stick constraints

references:
- https://www.flipcode.com/archives/Octree_Implementation.shtml, inspiration for using centered-aabb
- https://pvigier.github.io/2019/08/04/quadtree-collision-detection.html inspo for placing shared nodes only in parent