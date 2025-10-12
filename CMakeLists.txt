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
    src/utils/shaderUtils.cpp
    src/core/gameLogic.cpp
    src/model/torus.cpp
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
