#include "application.hpp"

int main()
{
    Application app;
    app.run();
    app.shutdown();

    return 0;
}

// architecture
// Application: init, run, and end
// Quadtree: class that stores particles and stuff
// Particle: struct or class, and draw
// AABB: class with intersects