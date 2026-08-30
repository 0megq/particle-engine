#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>
#include "raylib-cpp.hpp"

struct Particle;

class Application {
    private:
        const int m_screenWidth;
        const int m_screenHeight;

        raylib::Window m_window;
        raylib::Camera3D m_camera;
        std::vector<Particle> m_particles;

        void render();

    public:
        Application(int screenWidth = 800, int screenHeight = 800);
        void run();
        void shutdown();
};

#endif APPLICATION_H