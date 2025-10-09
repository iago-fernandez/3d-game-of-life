#include <cstdio>
#include <vector>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "../include/utils/shaderUtils.h"

const int SCR_WIDTH = 800, SCR_HEIGHT = 800;
GLint deadColorLoc;
GLint aliveColorLoc;
GLint gridSizeLoc;

static void glfw_error_callback(int code, const char* desc) {
    std::fprintf(stderr, "GLFW error %d: %s\n", code, desc);
}

static void framebuffer_size_callback(GLFWwindow*, int w, int h) {
    glViewport(0, 0, w, h);
}

int main() {

    // GLFW initialization
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        std::fprintf(stderr, "Failed to init GLFW\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Window creation and setup
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Game of Life - Simulator", nullptr, nullptr);
    if (!window) {
        std::fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    // GLAD initialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::fprintf(stderr, "Failed to init GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    // TODO callbacks initialization
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // OpenGL initialization
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClearDepth(1.0f);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // 2D Shading
    GLuint program2d = 0;
    try {
        program2d = makeProgramFromFiles(SHADER_DIR "/shader2d.vert", SHADER_DIR "/shader2d.frag");
    }
    catch (const std::exception& e) {
        std::fprintf(stderr, "%s\n", e.what());
        return 1;
    }
    glUseProgram(program2d);

    gridSizeLoc = glGetUniformLocation(program2d, "gridSize");
    deadColorLoc = glGetUniformLocation(program2d, "deadColor");
    aliveColorLoc = glGetUniformLocation(program2d, "aliveColor");

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glUniform2i(gridSizeLoc, 10, 10);
    glUniform3f(deadColorLoc, 0.92f, 0.92f, 0.92f);
    glUniform3f(aliveColorLoc, 0.12f, 0.12f, 0.12f);

    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program2d);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program2d);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
