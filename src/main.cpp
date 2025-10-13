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
#include "../include/core/gameLogic.h"
#include "../include/model/torus.h"

// Simulation control
bool   simRunning = false;
float  simStepsPerSec = 5.0f;
double simAccumulator = 0.0;
double simLastTime = 0.0;

struct OrbitCamera {

    glm::vec3 target = { 0.0f, 0.0f, 0.0f };
    float distance = 15.0f;
    float yaw = 0.0f;
    float pitch = glm::radians(-20.0f);

    glm::mat4 view() const {

        glm::vec3 dir = { cosf(pitch) * cosf(yaw), sinf(pitch), cosf(pitch) * sinf(yaw) };
        glm::vec3 eye = target - dir * distance;

        return glm::lookAt(eye, target, { 0.0f, 1.0f, 0.0f });
    }
};

OrbitCamera camera;

bool rotating3D = false;
double lastX3D = 0.0, lastY3D = 0.0;
double scrollDelta3D = 0.0;

static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {

    scrollDelta3D += yOffset;
}

int screenWidth = 1600;
int screenHeight = 800;
const int GRID_WIDTH = 50;
const int GRID_HEIGHT = 50;

Life life(GRID_WIDTH, GRID_HEIGHT);

// 2D uniforms
GLint uGridSize2DLoc;
GLint uViewportPx2DLoc;
GLint uState2DLoc;
GLint uDeadColor2DLoc;
GLint uAliveColor2DLoc;
GLint uLineThicknessPx2DLoc;
GLint uLineColor2DLoc;
GLint uHoverCell2DLoc;
GLint uHoverBoost2DLoc;
GLint uEdgeUColor2DLoc;
GLint uEdgeVColor2DLoc;
GLint uEdgeThicknessPx2DLoc;

// 3D uniforms
GLint uMVP3DLoc;
GLint uState3DLoc;
GLint uDeadColor3DLoc;
GLint uAliveColor3DLoc;
GLint uGridSize3DLoc;
GLint uLinePx3DLoc;
GLint uLineColor3DLoc;
GLint uEdgeUColor3DLoc;
GLint uEdgeVColor3DLoc;
GLint uEdgePxUV3DLoc;

GLuint stateTex = 0;

bool mouseDownPrev = false;
bool spacePrev = false;
bool cPrev = false;

static void glfw_error_callback(int code, const char* desc) {

    std::fprintf(stderr, "GLFW error %d: %s\n", code, desc);
}

static void framebuffer_size_callback(GLFWwindow*, int w, int h) {

    glViewport(0, 0, w, h);
}

int main() {

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) { std::fprintf(stderr, "Failed to init GLFW\n"); return 1; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Game of Life", nullptr, nullptr);
    if (!window) { std::fprintf(stderr, "Failed to create window\n"); glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::fprintf(stderr, "Failed to init GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    simLastTime = glfwGetTime();

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glViewport(0, 0, screenWidth, screenHeight);
    glClearDepth(1.0f);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // 2D program
    GLuint program2d = 0;
    try {
        program2d = makeProgramFromFiles(SHADER_DIR "/shader2d.vert", SHADER_DIR "/shader2d.frag");
    }
    catch (const std::exception& e) { std::fprintf(stderr, "%s\n", e.what()); return 1; }

    glUseProgram(program2d);
    uGridSize2DLoc = glGetUniformLocation(program2d, "uGridSize");
    uViewportPx2DLoc = glGetUniformLocation(program2d, "uViewportPx");
    uState2DLoc = glGetUniformLocation(program2d, "uState");
    uDeadColor2DLoc = glGetUniformLocation(program2d, "uDeadColor");
    uAliveColor2DLoc = glGetUniformLocation(program2d, "uAliveColor");
    uLineThicknessPx2DLoc = glGetUniformLocation(program2d, "uLineThicknessPx");
    uLineColor2DLoc = glGetUniformLocation(program2d, "uLineColor");
    uHoverCell2DLoc = glGetUniformLocation(program2d, "uHoverCell");
    uHoverBoost2DLoc = glGetUniformLocation(program2d, "uHoverBoost");
    uEdgeUColor2DLoc = glGetUniformLocation(program2d, "uEdgeUColor");
    uEdgeVColor2DLoc = glGetUniformLocation(program2d, "uEdgeVColor");
    uEdgeThicknessPx2DLoc = glGetUniformLocation(program2d, "uEdgeThicknessPx");

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenTextures(1, &stateTex);
    glBindTexture(GL_TEXTURE_2D, stateTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, GRID_WIDTH, GRID_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, life.data());

    glUniform2i(uGridSize2DLoc, GRID_WIDTH, GRID_HEIGHT);
    glUniform3f(uDeadColor2DLoc, 0.92f, 0.92f, 0.92f);
    glUniform3f(uAliveColor2DLoc, 0.12f, 0.12f, 0.12f);
    glUniform1f(uLineThicknessPx2DLoc, 0.5f);
    glUniform3f(uLineColor2DLoc, 0.76f, 0.76f, 0.76f);
    glUniform1f(uHoverBoost2DLoc, 0.25f);
    glUniform1i(uState2DLoc, 0);
    glUniform3f(uEdgeUColor2DLoc, 0.30f, 0.50f, 1.00f);
    glUniform3f(uEdgeVColor2DLoc, 1.00f, 0.35f, 0.35f);
    glUniform1f(uEdgeThicknessPx2DLoc, 1.5f);

    auto mouse_to_cell = [&](double mx, double my, int vpX, int vpY, int vpW, int vpH, int& cx, int& cy) {
        double lx = mx - (double)vpX;
        double ly = my - (double)vpY;
        if (lx < 0.0 || ly < 0.0 || lx >= vpW || ly >= vpH) { cx = cy = -1; return; }
        cx = (int)(lx * GRID_WIDTH / (double)vpW);
        cy = (int)(((double)vpH - 1.0 - ly) * GRID_HEIGHT / (double)vpH);
        if (cx < 0 || cy < 0 || cx >= GRID_WIDTH || cy >= GRID_HEIGHT) { cx = cy = -1; }
        };

    // Step simulation + upload to texture
    auto step_once = [&]() {
        life.step();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, stateTex);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
            GRID_WIDTH, GRID_HEIGHT,
            GL_RED, GL_UNSIGNED_BYTE, life.data());
        };

    // 3D program
    GLuint programTorus = 0;
    try {
        programTorus = makeProgramFromFiles(SHADER_DIR "/shader3d.vert", SHADER_DIR "/shader3d.frag");
    }
    catch (const std::exception& e) { std::fprintf(stderr, "%s\n", e.what()); return 1; }

    auto torus = makeTorusGrid(GRID_WIDTH, GRID_HEIGHT, 2.0f, 0.7f);

    uMVP3DLoc = glGetUniformLocation(programTorus, "uMVP");
    uState3DLoc = glGetUniformLocation(programTorus, "uState");
    uDeadColor3DLoc = glGetUniformLocation(programTorus, "uDeadColor");
    uAliveColor3DLoc = glGetUniformLocation(programTorus, "uAliveColor");
    uGridSize3DLoc = glGetUniformLocation(programTorus, "uGridSize");
    uLinePx3DLoc = glGetUniformLocation(programTorus, "uLinePx");
    uLineColor3DLoc = glGetUniformLocation(programTorus, "uLineColor");
    uEdgeUColor3DLoc = glGetUniformLocation(programTorus, "uEdgeUColor");
    uEdgeVColor3DLoc = glGetUniformLocation(programTorus, "uEdgeVColor");
    uEdgePxUV3DLoc = glGetUniformLocation(programTorus, "uEdgePxUV");

    const int kToolbarMargin = 12;
    const ImVec2 kBtnSize(64.0f, 40.0f);

    while (!glfwWindowShouldClose(window)) {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
        const int leftViewportWidth = screenWidth / 2;
        const int rightViewportWidth = screenWidth - leftViewportWidth;

        const double now = glfwGetTime();
        double dt = now - simLastTime;
        simLastTime = now;
        if (dt > 0.25) dt = 0.25;
        if (simRunning) {
            simAccumulator += dt;
            const double stepPeriod = 1.0 / glm::max(0.0001f, simStepsPerSec);
            int steps = 0;
            while (simAccumulator >= stepPeriod && steps < 240) {
                step_once();
                simAccumulator -= stepPeriod;
                ++steps;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double mx, my;
        glfwGetCursorPos(window, &mx, &my);

        {
            ImGui::SetNextWindowBgAlpha(0.55f);
            ImGui::SetNextWindowPos(
                ImVec2((float)(leftViewportWidth + rightViewportWidth - kToolbarMargin), (float)(screenHeight - kToolbarMargin)),
                ImGuiCond_Always, ImVec2(1.0f, 1.0f));

            ImGui::Begin("Toolbar", nullptr,
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_AlwaysAutoResize);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 12));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

            const char* playLabel = simRunning ? "Pause" : "Play";
            if (ImGui::Button(playLabel, kBtnSize)) {
                simRunning = !simRunning;
                if (simRunning) {
                    simAccumulator = 0.0;
                    simLastTime = glfwGetTime();
                }
            }
            ImGui::SameLine();

            if (ImGui::Button("Step", kBtnSize)) {
                step_once();
            }
            ImGui::SameLine();

            ImGui::SetNextItemWidth(220.0f);
            ImGui::SliderFloat("##Speed", &simStepsPerSec, 0.5f, 10.0f, "Speed: %.1f");

            ImGui::PopStyleVar(2);
            ImGui::End();
        }

        bool uiBlocksMouse = ImGui::GetIO().WantCaptureMouse;

        // 2D (left viewport)
        glDisable(GL_DEPTH_TEST);
        glViewport(0, 0, leftViewportWidth, screenHeight);

        int hx = -1, hy = -1;
        if (!uiBlocksMouse) {
            mouse_to_cell(mx, my, 0, 0, leftViewportWidth, screenHeight, hx, hy);
        }

        glUseProgram(program2d);
        glUniform2f(uViewportPx2DLoc, (float)leftViewportWidth, (float)screenHeight);
        glUniform2i(uGridSize2DLoc, GRID_WIDTH, GRID_HEIGHT);
        glUniform3f(uDeadColor2DLoc, 0.92f, 0.92f, 0.92f);
        glUniform3f(uAliveColor2DLoc, 0.12f, 0.12f, 0.12f);
        glUniform1f(uLineThicknessPx2DLoc, 0.5f);
        glUniform3f(uLineColor2DLoc, 0.76f, 0.76f, 0.76f);
        glUniform1f(uHoverBoost2DLoc, 0.25f);
        glUniform1i(uState2DLoc, 0);
        glUniform2i(uHoverCell2DLoc, hx, hy);
        glUniform3f(uEdgeUColor2DLoc, 0.30f, 0.50f, 1.00f);
        glUniform3f(uEdgeVColor2DLoc, 1.00f, 0.35f, 0.35f);
        glUniform1f(uEdgeThicknessPx2DLoc, 1.5f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, stateTex);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Grid interaction (left viewport only, not through UI)
        bool mouseDown = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
        if (!uiBlocksMouse && mouseDown && !mouseDownPrev && hx >= 0 && hy >= 0) {
            uint8_t& v = life.at(hx, hy);
            v ^= 1;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, stateTex);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexSubImage2D(GL_TEXTURE_2D, 0, hx, hy, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &v);
        }
        mouseDownPrev = mouseDown;

        bool spaceNow = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
        if (spaceNow && !spacePrev) {
            life.step();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, stateTex);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GRID_WIDTH, GRID_HEIGHT, GL_RED, GL_UNSIGNED_BYTE, life.data());
        }
        spacePrev = spaceNow;

        bool cNow = (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS);
        if (cNow && !cPrev) {
            life.clear();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, stateTex);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GRID_WIDTH, GRID_HEIGHT, GL_RED, GL_UNSIGNED_BYTE, life.data());
        }
        cPrev = cNow;

        // 3D (right viewport)
        glEnable(GL_DEPTH_TEST);
        glViewport(leftViewportWidth, 0, rightViewportWidth, screenHeight);

        bool inRight = (mx >= leftViewportWidth && mx < (leftViewportWidth + rightViewportWidth) && my >= 0 && my < screenHeight) && !uiBlocksMouse;

        bool L = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
        if (!inRight) rotating3D = false;
        if (inRight && L && !rotating3D) {
            rotating3D = true;
            glfwGetCursorPos(window, &lastX3D, &lastY3D);
        }
        if (!L) rotating3D = false;

        double cx, cy;
        glfwGetCursorPos(window, &cx, &cy);
        double dx3 = cx - lastX3D;
        double dy3 = cy - lastY3D;

        if (inRight && rotating3D) {
            const float rotSpeed = 0.005f;
            camera.yaw += (float)dx3 * rotSpeed;
            camera.pitch -= (float)dy3 * rotSpeed;
            const float limit = glm::radians(89.0f);
            camera.pitch = glm::clamp(camera.pitch, -limit, limit);
            lastX3D = cx; lastY3D = cy;
        }

        if (!inRight || uiBlocksMouse) {
            scrollDelta3D = 0.0;
        }

        if (inRight && scrollDelta3D != 0.0) {
            camera.distance *= std::exp((float)(-scrollDelta3D) * 0.15f);
            camera.distance = glm::clamp(camera.distance, 1.0f, 200.0f);
            scrollDelta3D = 0.0;
        }

        const float aspect = (rightViewportWidth > 0) ? (float)rightViewportWidth / (float)screenHeight : 1.0f;
        glm::mat4 proj = glm::perspective(glm::radians(25.0f), aspect, 0.1f, 200.0f);
        glm::mat4 view = camera.view();
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 mvp = proj * view * model;

        glUseProgram(programTorus);
        glUniformMatrix4fv(uMVP3DLoc, 1, GL_FALSE, &mvp[0][0]);
        glUniform1i(uState3DLoc, 0);
        glUniform2i(uGridSize3DLoc, GRID_WIDTH, GRID_HEIGHT);
        glUniform3f(uDeadColor3DLoc, 1.0f, 1.0f, 1.0f);
        glUniform3f(uAliveColor3DLoc, 0.0f, 0.0f, 0.0f);
        glUniform1f(uLinePx3DLoc, 0.7f);
        glUniform3f(uLineColor3DLoc, 0.75f, 0.75f, 0.75f);
        glUniform3f(uEdgeUColor3DLoc, 0.30f, 0.50f, 1.00f);
        glUniform3f(uEdgeVColor3DLoc, 1.00f, 0.35f, 0.35f);
        glUniform1f(uEdgePxUV3DLoc, 2.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, stateTex);

        glBindVertexArray(torus.vao);
        glDrawElements(GL_TRIANGLES, torus.indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Render UI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program2d);
    destroyTorus(torus);
    glDeleteProgram(programTorus);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
