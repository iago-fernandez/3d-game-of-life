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

int SCR_WIDTH = 800;
int SCR_HEIGHT = 800;
const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 20;
std::vector<uint8_t> grid(GRID_WIDTH * GRID_HEIGHT, 0);

GLint gridSizeLoc;
GLint viewportPxLoc;
GLint stateLoc;
GLint deadColorLoc;
GLint aliveColorLoc;
GLint lineThicknessPxLoc;
GLint lineColorLoc;
GLint hoverCellLoc;
GLint hoverBoostLoc;
GLuint stateTex = 0;

bool mouseDownPrev = false;

static void glfw_error_callback(int code, const char* desc) {
    std::fprintf(stderr, "GLFW error %d: %s\n", code, desc);
}

static void framebuffer_size_callback(GLFWwindow*, int w, int h) {
    glViewport(0, 0, w, h);
}

auto mouse_to_cell = [&](double mx, double my, int& cx, int& cy) {

    // Convert window mouse coords to cell indices
    cx = static_cast<int>(mx * GRID_WIDTH / SCR_WIDTH);
    cy = static_cast<int>((SCR_HEIGHT - 1 - my) * GRID_HEIGHT / SCR_HEIGHT);    // GLFW origin for y is top-left for cursor; flip to bottom-left

    if (cx < 0 || cy < 0 || cx >= GRID_WIDTH || cy >= GRID_HEIGHT) {
        cx = cy = -1;
    }
};

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Game of Life", nullptr, nullptr);
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
    viewportPxLoc = glGetUniformLocation(program2d, "viewportPx");
    stateLoc = glGetUniformLocation(program2d, "state");
    deadColorLoc = glGetUniformLocation(program2d, "deadColor");
    aliveColorLoc = glGetUniformLocation(program2d, "aliveColor");
    lineThicknessPxLoc = glGetUniformLocation(program2d, "lineThicknessPx");
    lineColorLoc = glGetUniformLocation(program2d, "lineColor");
    hoverCellLoc = glGetUniformLocation(program2d, "hoverCell");
    hoverBoostLoc = glGetUniformLocation(program2d, "hoverBoost");

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenTextures(1, &stateTex);
    glBindTexture(GL_TEXTURE_2D, stateTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // important for GL_R8
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, GRID_WIDTH, GRID_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, grid.data());
    
    glUniform2i(gridSizeLoc, GRID_WIDTH, GRID_HEIGHT);
    glUniform3f(deadColorLoc, 0.92f, 0.92f, 0.92f);
    glUniform3f(aliveColorLoc, 0.12f, 0.12f, 0.12f);
    glUniform1f(lineThicknessPxLoc, 0.5f);
    glUniform3f(lineColorLoc, 0.76f, 0.76f, 0.76f);
    glUniform1f(hoverBoostLoc, 0.25f);
    glUniform1i(stateLoc, 0);

    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Framebuffer size and viewport uniform
        glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glUseProgram(program2d);
        glUniform2f(viewportPxLoc, (float)SCR_WIDTH, (float)SCR_HEIGHT);

        // Cell hover from mouse position
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        int hx = -1, hy = -1;
        mouse_to_cell(mx, my, hx, hy);
        glUniform2i(hoverCellLoc, hx, hy);

        // Click-to-toggle (single texel update)
        bool mouseDown = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
        if (mouseDown && !mouseDownPrev && hx >= 0 && hy >= 0) {
            uint8_t &v = grid[hy * GRID_WIDTH + hx];
            v ^= 1; // toggle 0 <-> 1

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, stateTex);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexSubImage2D(GL_TEXTURE_2D, 0, hx, hy, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &v);
        }
        mouseDownPrev = mouseDown;

        // Bind texture and draw
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, stateTex);

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
