#include <raylib-cpp.hpp>
#include <raylib-cpp-utils.hpp>
#include <vector>


void SphereInstancingDemo() {
    raylib::Mesh sphere = raylib::Mesh::Sphere(1.0f, 8, 8);

    const int count = 10;
    raylib::Matrix transforms[count];
    transforms[0] = raylib::Matrix::Identity();
    for (int i = 1; i < count; i++) {
        transforms[i] = raylib::Matrix::Translate(1.0f, 0.0f, 1.0f) * transforms[i - 1];
    }

    // possibly an issue with this here
    sphere.Draw(LoadMaterialDefault(), transforms, count);
}

