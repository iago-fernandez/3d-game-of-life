#include "../../include/app/app.h"

#include "../../include/app/input.h"
#include "../../include/core/simulation.h"
#include "../../include/core/camera.h"
#include "../../include/render/renderer2d.h"
#include "../../include/render/renderer3d.h"
#include "../../include/ui/toolbar.h"

#include <cstdio>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using app::App;

// GLFW trampolines
static void glfwErrorCb(int code, const char* desc) {
    std::fprintf(stderr, "GLFW error %d: %s\n", code, desc);
}

static void framebufferCb(GLFWwindow* w, int width, int height) {
    App* self = reinterpret_cast<App*>(glfwGetWindowUserPointer(w));
    if (self) self->onResize(width, height);
}

static void scrollCb(GLFWwindow* w, double /*xoff*/, double yoff) {
    App* self = reinterpret_cast<App*>(glfwGetWindowUserPointer(w));
    if (self) self->onScroll(yoff);
}

namespace app {

    App::App(const AppConfig& cfg) : config_(cfg) {}
    App::~App() {}

    bool App::init() {
        if (!initGlfw()) return false;
        if (!initGlad()) return false;
        if (!initImgui()) return false;

        lastTime_ = glfwGetTime();

        // Minimal baseline GL state
        glClearDepth(1.0f);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        // Systems
        simulation_ = std::make_unique<core::Simulation>(50, 50);
        camera_ = std::make_unique<core::OrbitCamera>();
        r2d_ = std::make_unique<render::Renderer2D>(*simulation_);
        r3d_ = std::make_unique<render::Renderer3D>(*simulation_);
        toolbarState_ = std::make_unique<ui::ToolbarState>();
        input_ = std::make_unique<InputState>();

        onResize(config_.windowWidth, config_.windowHeight);
        return true;
    }

    bool App::initGlfw() {
        glfwSetErrorCallback(glfwErrorCb);
        if (!glfwInit()) { std::fprintf(stderr, "Failed to init GLFW\n"); return false; }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window_ = glfwCreateWindow(config_.windowWidth, config_.windowHeight, config_.title.c_str(), nullptr, nullptr);
        if (!window_) {
            std::fprintf(stderr, "Failed to create window\n");
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(window_);

        // Store instance pointer for callbacks
        glfwSetWindowUserPointer(window_, this);
        glfwSetFramebufferSizeCallback(window_, framebufferCb);
        glfwSetScrollCallback(window_, scrollCb);
        return true;
    }

    bool App::initGlad() {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::fprintf(stderr, "Failed to init GLAD\n");
            return false;
        }
        return true;
    }

    bool App::initImgui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window_, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        return true;
    }

    void App::shutdownImgui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void App::shutdown() {
        r3d_.reset();
        r2d_.reset();
        camera_.reset();
        simulation_.reset();

        shutdownImgui();

        if (window_) {
            glfwDestroyWindow(window_);
            window_ = nullptr;
        }
        glfwTerminate();
    }

    void App::onResize(int width, int height) {
        fbWidth_ = width;
        fbHeight_ = height;
        glViewport(0, 0, width, height);
    }

    void App::onScroll(double yoff) {
        // Accumulate to be consumed once per frame by the camera
        scrollDelta_ += yoff;
    }

    void App::updateInput() {
        double mx, my;
        glfwGetCursorPos(window_, &mx, &my);

        input_->mouseX_ = mx;
        input_->mouseY_ = my;
        input_->mouseL_ = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        input_->wantCaptureMouse_ = ImGui::GetIO().WantCaptureMouse;
    }

    void App::simulate(double dt) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ui::ToolbarActions act = ui::drawToolbar(*toolbarState_, *simulation_);

        if (act.toggledRun) simulation_->toggleRun();
        if (act.requestStep) simulation_->stepOnce();
        if (act.requestClear) simulation_->clear();
        if (act.newSpeed > 0.0f) simulation_->setStepsPerSecond(act.newSpeed);

        if (act.resizeCols >= 0 || act.resizeRows >= 0) {
            const int cols = (act.resizeCols >= 0) ? act.resizeCols : simulation_->width();
            const int rows = (act.resizeRows >= 0) ? act.resizeRows : simulation_->height();
            simulation_->resize(cols, rows);
            r3d_->rebuildMesh(cols, rows);
        }

        // Fixed-timestep advance (accumulator lives inside Simulation)
        simulation_->advance(dt);

        ImGui::Render();
    }

    void App::draw2D() {
        const int leftW = fbWidth_ / 2;

        glDisable(GL_DEPTH_TEST);
        glViewport(0, 0, leftW, fbHeight_);

        int hx = -1, hy = -1;
        if (!input_->wantCaptureMouse_) {
            render::mouseToCell(input_->mouseX_, input_->mouseY_, 0, 0, leftW, fbHeight_, simulation_->width(), simulation_->height(), hx, hy);
        }

        r2d_->draw(leftW, fbHeight_, hx, hy);

        // Edge-triggered toggling to avoid repeats while holding the mouse
        static bool prevDown = false;
        if (input_->mouseL_ && !prevDown && hx >= 0 && hy >= 0) {
            simulation_->toggleCell(hx, hy);
        }
        prevDown = input_->mouseL_;
    }

    void App::draw3D() {
        const int leftW = fbWidth_ / 2;
        const int rightW = fbWidth_ - leftW;

        glEnable(GL_DEPTH_TEST);
        glViewport(leftW, 0, rightW, fbHeight_);

        // Interact only when mouse is inside right viewport and not over UI
        const bool inRight = (input_->mouseX_ >= leftW && input_->mouseX_ < (double)(leftW + rightW) && input_->mouseY_ >= 0.0 && input_->mouseY_ < (double)fbHeight_) && !input_->wantCaptureMouse_;

        static bool rotating = false;
        static double lastX = 0.0, lastY = 0.0;

        if (!inRight) rotating = false;
        if (inRight && input_->mouseL_ && !rotating) {
            rotating = true;
            lastX = input_->mouseX_; lastY = input_->mouseY_;
        }
        if (!input_->mouseL_) rotating = false;

        if (inRight && rotating) {
            const double dx = input_->mouseX_ - lastX;
            const double dy = input_->mouseY_ - lastY;
            camera_->orbitBy((float)dx, (float)dy);
            lastX = input_->mouseX_; lastY = input_->mouseY_;
        }

        if (inRight && scrollDelta_ != 0.0) {
            camera_->zoomBy((float)scrollDelta_);
            scrollDelta_ = 0.0;
        }
        else if (!inRight) {
            scrollDelta_ = 0.0;
        }

        r3d_->draw(*camera_, rightW, fbHeight_);
    }

    void App::run() {
        while (!glfwWindowShouldClose(window_)) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            double now = glfwGetTime();
            double dt = now - lastTime_;
            lastTime_ = now;
            if (dt > 0.25) dt = 0.25;  // clamp to avoid huge steps after stalls

            updateInput();
            simulate(dt);
            draw2D();
            draw3D();

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window_);
            glfwPollEvents();
        }
    }

}
