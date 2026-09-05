# Particle engine in C++

Uses raylib library. CMakeLists.txt is a modified version from [raylib-cpp's cmake template](https://github.com/RobLoach/raylib-cpp/blob/master/projects/CMake/CMakeLists.txt)


# interesting things to highlight:
- verlet integration
- substepping

# planned
- optimize collisions
  - broadphase (octree)
  - sleeping still particles
  - sweep and prune with insertion sort (insertion sort because of temporal coherence)
    - optimized SAP method: https://ieeexplore.ieee.org/document/10121435
- moving around with camera
- stick constraints