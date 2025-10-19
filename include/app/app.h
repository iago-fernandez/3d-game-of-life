#pragma once

#include <memory>
#include <string>

struct GLFWwindow;

namespace core { class Simulation; class OrbitCamera; }
namespace render { class Renderer2D; class Renderer3D; }
namespace ui { struct ToolbarState; struct ToolbarActions; }
namespace app { struct InputState; }

namespace app {

    /**
     * @brief Window and title configuration for the application.
     */
    struct AppConfig {
        int windowWidth = 1800;
        int windowHeight = 900;
        std::string title = "Game Of Life";
    };

    /**
     * @brief Main application. Owns window, GL context, UI and the main loop.
     */
    class App {
    public:
        explicit App(const AppConfig& cfg);
        ~App();

        /**
         * @brief Initialize window, OpenGL loader and ImGui.
         * @return True on success, false otherwise.
         */
        bool init();

        /**
         * @brief Run the main loop until the window closes.
         */
        void run();

        /**
         * @brief Release resources and terminate subsystems.
         */
        void shutdown();

        /**
         * @brief Framebuffer resize handler used by GLFW.
         * @param width New framebuffer width.
         * @param height New framebuffer height.
         */
        void onResize(int width, int height);

        /**
         * @brief Scroll handler used by GLFW (Y axis).
         * @param yoff Scroll delta on Y.
         */
        void onScroll(double yoff);

    private:
        // Non-copyable
        App(const App&) = delete;
        App& operator=(const App&) = delete;

        // Init and teardown
        bool initGlfw();
        bool initGlad();
        bool initImgui();
        void shutdownImgui();

        // Per-frame stages
        void updateInput();
        void simulate(double dt);
        void draw2D();
        void draw3D();

    private:
        AppConfig config_{};
        GLFWwindow* window_ = nullptr;

        int fbWidth_ = 0;
        int fbHeight_ = 0;

        std::unique_ptr<core::Simulation> simulation_;
        std::unique_ptr<core::OrbitCamera> camera_;
        std::unique_ptr<render::Renderer2D> r2d_;
        std::unique_ptr<render::Renderer3D> r3d_;
        std::unique_ptr<ui::ToolbarState> toolbarState_;
        std::unique_ptr<InputState> input_;

        double lastTime_ = 0.0;
        double scrollDelta_ = 0.0;
    };

}
