#include <raylib-cpp.hpp>
#include <raylib-cpp-utils.hpp>
#include <vector>


void SphereInstancingDemo(bool shutdown) {
    static const int instanceCount = 10;

    static raylib::Shader shader;
    static raylib::Matrix transforms[instanceCount];
    static raylib::Mesh sphere;
    static raylib::Material material;

    static bool initialized = false;

    if (shutdown) {
        material.Unload();
        shader.Unload();
        sphere.Unload();
    }
    
    if (!initialized) {
        initialized = true;

        sphere = raylib::Mesh::Sphere(1.0f, 8, 8);

        transforms[0] = raylib::Matrix::Identity();
        for (int i = 1; i < instanceCount; i++) {
            transforms[i] = raylib::Matrix::Translate(1.0f, 0.0f, 1.0f) * transforms[i - 1];
        }

        shader = raylib::Shader::Load("res/base_instance.vs", "res/flat.fs");
        shader.locs[SHADER_LOC_MATRIX_MVP] = shader.GetLocation("mvp");

        material = LoadMaterialDefault();
        material.shader = shader;
    }

    sphere.Draw(material, transforms, instanceCount);
}


