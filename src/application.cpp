#include <iostream>
#include <random>
#include "application.hpp"
#include "particle.hpp"


Application::Application(int screenWidth, int screenHeight) :
    m_screenWidth(screenWidth),
    m_screenHeight(screenHeight), 
    m_window(screenWidth, screenHeight, "Particle Engine") 
{
    m_camera.SetPosition({-70.0f, 5.0f, 0.0f});
    m_camera.SetTarget({});
    m_camera.SetUp({0.0f, 1.0f, 0.0f});
    // m_particleSystem.addParticle({10.0f, 5.0f, 8.0f}, {}, fixedUpdateDelta);

}

void Application::run() {
    while (!m_window.ShouldClose()) {
        auto dt = m_window.GetFrameTime();

        // Update particles
        m_fixedUpdateAccumulator += dt;
        while (m_fixedUpdateAccumulator > fixedUpdateDelta) {
            if (m_fixedFrameCount % 5 == 0) {
                m_particleSystem.addParticle({5.0f, 5.0f, (float)GetRandomValue(4,6)}, {}, fixedUpdateDelta, raylib::Color((char)GetRandomValue(0,255), (char)GetRandomValue(0,255), (char)GetRandomValue(0,255)), 0.5f);
            }
            m_fixedUpdateAccumulator -= fixedUpdateDelta;
            m_particleSystem.update(fixedUpdateDelta * 4);
            m_fixedFrameCount++;
        }

        // Draw particles
        render();
        m_frameCount++;
    }
}

void Application::render() {
    m_window.BeginDrawing();
    m_window.ClearBackground(BLACK);

    m_camera.BeginMode();

    m_particleSystem.render();

    m_camera.EndMode();

    m_window.DrawFPS(10, 10);
    m_window.EndDrawing();
}

void Application::shutdown() {
    m_window.Close();
}
