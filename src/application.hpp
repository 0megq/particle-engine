#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>
#include "raylib-cpp.hpp"
#include "particle_system.hpp"


class Application {
    private:
        static constexpr float fixedUpdateDelta = 1.0f / 60.0f;

        const int m_screenWidth;
        const int m_screenHeight;

        int m_frameCount = 0;
        int m_fixedFrameCount = 0;
        float m_fixedUpdateAccumulator = 0.0f;
        ParticleSystem m_particleSystem;
        raylib::Window m_window;
        raylib::Camera3D m_camera;

        void render();

    public:
        Application(int screenWidth = 800, int screenHeight = 800);
        void run();
        void shutdown();
};

#endif APPLICATION_H