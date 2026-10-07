# Particle engine in C++ 17

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
    - https://leanrada.com/notes/sweep-and-prune-2/
  - store particles as a structure of arrays for cache efficiency
- moving around with camera
- stick constraints
- forces on particles
  - press a button, or mouse
  - shake the window

references:
- https://www.flipcode.com/archives/Octree_Implementation.shtml, inspiration for using centered-aabb
- https://pvigier.github.io/2019/08/04/quadtree-collision-detection.html inspo for placing shared nodes only in parent
- https://leanrada.com/notes/sweep-and-prune/

how i'm fixing the octree:
my first guess was that the octree was not really partitioning at all and all the children were being grouped into a single node

then in order to dig into this i started profiling the game code, getting stats on how many collisions were happening, and then logging data at different stages of the octree

first i found that all particles were in the root node using some basic logging. then i had to figure out why this was happening?

I visualized octree and in fact only saw 2 layers of octrees (the root and its children). when the octree was trying to insert it would fail to insert into its children. i digged into this further and further. ultimately it came down to a single line of code that was causing the function used for identifiyng the octant of a particle to always return -1 (i.e. that no octant completely contains that particle). I had written vec.z instead of vec.z == 0

okay i fixed it, but now the simulation seems to get highly unstable where particles are bouncing excessively

i imagine this is because certain collisions aren't being detected, because a particle will be updated within the loop putting it into a colliding state with another particle. However, since that pair was not within the previous octree it fails.

I will now instead just directly check for intersection in the octree and only do resolution in the particle system. this is similar to an approach used by [one of my references ](https://pvigier.github.io/2019/08/04/quadtree-collision-detection.html).