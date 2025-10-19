cmake_minimum_required(VERSION 3.20)
project(GameOfLife LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(GameOfLife
    src/main.cpp
    src/app/app.cpp
    src/app/input.cpp
    src/core/camera.cpp
    src/core/gameLogic.cpp
    src/core/simulation.cpp
    src/model/torus.cpp
    src/render/renderer2d.cpp
    src/render/renderer3d.cpp
    src/ui/toolbar.cpp
    src/utils/shaderUtils.cpp
)

target_include_directories(GameOfLife PRIVATE src include)

target_link_libraries(GameOfLife PRIVATE
    glfw
    glad::glad
    glm::glm
    imgui::imgui
    OpenGL::GL
)

target_compile_definitions(GameOfLife PRIVATE SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders")

if (MSVC)
  target_compile_options(GameOfLife PRIVATE /W4 /permissive-)
endif()
