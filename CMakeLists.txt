cmake_minimum_required(VERSION 3.20)
project(particle-engine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# Fetch Raylib 6.0
FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF  # Disables CMake 3.24+ file modification timestamp warnings
    URL https://github.com/raysan5/raylib/archive/refs/tags/6.0.tar.gz
)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
    raylib_cpp
    GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
    GIT_TAG ${RAYLIB_VERSION}
)

FetchContent_MakeAvailable(raylib_cpp)

# Automatically find all C++ source files in src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
set(INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/src/")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src ext/raylib-cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)