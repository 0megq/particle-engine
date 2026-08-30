#include "raylib-cpp.hpp"
#include "application.hpp"
#include "particle.hpp"


Application::Application(int screenWidth, int screenHeight) :
    m_screenWidth(screenWidth),
    m_screenHeight(screenHeight), 
    m_window(screenWidth, screenHeight, "Particle Engine") 
{
    m_window.SetTargetFPS(60);
    m_camera.SetPosition(raylib::Vector3(0.0f, 10.0f, -10.0f));
    m_camera.SetTarget(raylib::Vector3::Zero());
    m_camera.SetUp(raylib::Vector3(0.0f, 1.0f, 0.0f));

    m_particles.push_back(Particle(raylib::Vector3(0.0f, 0.0f, 0.0f)));
}

void Application::run() {
    while (!m_window.ShouldClose()) {
        auto delta = m_window.GetFrameTime();
        // Update particles

        // Draw particles
        render();
    }
}

void Application::render() {
    m_window.BeginDrawing();
    m_window.ClearBackground(WHITE);

    m_camera.BeginMode();

    for (Particle &p : m_particles) {
        p.draw();
    }

    m_camera.EndMode();

    m_window.DrawFPS(10, 10);
    m_window.EndDrawing();
}

void Application::shutdown() {
    m_window.Close();
}