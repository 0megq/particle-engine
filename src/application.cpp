#include <iostream>
#include <random>
#include <tracy/Tracy.hpp>
#include "application.hpp"
#include "particle.hpp"


Application::Application(int screenWidth, int screenHeight) :
    m_screenWidth(screenWidth),
    m_screenHeight(screenHeight), 
    m_window(screenWidth, screenHeight, "Particle Engine") 
{
    m_camera.SetPosition({0.0f, 0.0f, 70.0f});
    m_camera.SetTarget({});
    m_camera.SetUp({0.0f, 1.0f, 0.0f});
    // m_particleSystem.addParticle({10.0f, 5.0f, 8.0f}, {}, fixedUpdateDelta);

}

void Application::run() {
    while (!m_window.ShouldClose()) {
        ZoneScopedN("main loop");
        auto dt = m_window.GetFrameTime();

        if (IsKeyDown(KEY_LEFT)) {
            m_hRotation += -dt;
        } if (IsKeyDown(KEY_RIGHT)){
            m_hRotation += dt;
        }

        if (IsKeyDown(KEY_UP)){
            m_vRotation += dt;
        }
        if (IsKeyDown(KEY_DOWN)){
            m_vRotation += -dt;
        }
        
        m_vRotation = std::min(PI / 2 - 0.05f, m_vRotation);
        m_vRotation = std::max(-PI / 2 + 0.05f, m_vRotation);

        // std::cout << m_hRotation << " " << m_vRotation << std::endl;
        
        m_camera.position = raylib::Vector3{
                                std::cos(m_vRotation) * std::sin(m_hRotation),
                                std::sin(m_vRotation),
                                std::cos(m_vRotation) * std::cos(m_hRotation)
                            } * 70.0f;

        // Update particles
        m_fixedUpdateAccumulator += dt;
        while (m_fixedUpdateAccumulator > fixedUpdateDelta) {
            ZoneScopedN("particle update");
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
        FrameMark;
    }
}

void Application::render() {
    ZoneScoped;
    m_window.BeginDrawing();
    m_window.ClearBackground(BLACK);

    m_camera.BeginMode();

    m_particleSystem.renderWorld();

    m_camera.EndMode();
    m_window.DrawFPS(10, 10);
    m_particleSystem.renderUI();
    {
        ZoneScopedN("End Drawing");
        m_window.EndDrawing();
    }
}

void Application::shutdown() {
    m_window.Close();
}
