#ifndef MESH_INSTANCE_RENDERER_H
#define MESH_INSTANCE_RENDERER_H
#include <raylib-cpp.hpp>
#include <raylib-cpp-utils.hpp>
#include <vector>
#include "particle.hpp"

class MeshInstanceRenderer {
    private:
        raylib::Shader shader;
        raylib::Material material;
        raylib::Mesh mesh;

    // Default constructor initializes
    // `shader` with base_instance.vs and flat.fs
    // `material` with the `shader` and raylib's default material
    // `mesh` with a sphere of radius 1
    public:
        MeshInstanceRenderer() {}
        
        void initialize() {
            shader = raylib::Shader::Load("res/base_instance.vs", "res/flat.fs");
            shader.locs[SHADER_LOC_MATRIX_MVP] = shader.GetLocation("mvp");
            material = LoadMaterialDefault();
            material.shader = shader;

            mesh = raylib::Mesh::Sphere(1.0f, 16, 16);
        }

        void render(const std::vector<raylib::Matrix> &transforms) {
            mesh.Draw(material, &transforms[0], transforms.size());
        }

        void renderParticles(const std::vector<Particle> &particles) {
            std::vector<raylib::Matrix> transforms;
            for (const auto &particle : particles){
                auto scaleMatrix = raylib::Matrix::Scale(particle.radius, particle.radius, particle.radius);
                auto translateMatrix = raylib::Matrix::Translate(particle.posCur.x, particle.posCur.y, particle.posCur.z);
                transforms.push_back(scaleMatrix * translateMatrix);
            }
            render(transforms);
        }

        ~MeshInstanceRenderer() {
            material.Unload();
            shader.Unload();
            mesh.Unload();
        }


        static void SphereInstancingDemo() {
            static const int instanceCount = 10;

            static MeshInstanceRenderer renderer;
            static std::vector<raylib::Matrix> transforms;

            static bool initialized = false;
            
            if (!initialized) {
                initialized = true;
                // set the transforms of each instance
                transforms.resize(instanceCount);
                transforms[0] = raylib::Matrix::Identity();
                for (int i = 1; i < instanceCount; i++) {
                    transforms[i] = raylib::Matrix::Translate(1.0f, 0.0f, 1.0f) * transforms[i - 1];
                }
            }

            renderer.render(transforms);
        }
};


#endif