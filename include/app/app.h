#pragma once

#include <memory>
#include <string>

struct GLFWwindow;

namespace core { class Simulation; class OrbitCamera; }
namespace render { class Renderer2D; class Renderer3D; }
namespace ui { struct ToolbarState; }
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
     * @brief Main application class.
     *
     * Owns the platform window, OpenGL context, and orchestrates the main loop.
     */
    class App {
    public:
        explicit App(const AppConfig& cfg);
        ~App();

        // Application instance is unique and tied to a single window context
        App(const App&) = delete;
        App& operator=(const App&) = delete;

        /**
         * @brief Initialize window, OpenGL loader, and UI context.
         * @return True on success, false otherwise.
         */
        bool init();

        /**
         * @brief Execute the main application loop.
         */
        void run();

        /**
         * @brief Release resources and terminate subsystems.
         */
        void shutdown();

        /**
         * @brief Callback handler for framebuffer resize events.
         * @param width New framebuffer width.
         * @param height New framebuffer height.
         */
        void onResize(int width, int height);

        /**
         * @brief Callback handler for scroll events.
         * @param yoff Scroll delta on the Y axis.
         */
        void onScroll(double yoff);

    private:
        bool initGlfw();
        bool initGlad();
        bool initImgui();
        void shutdownImgui();

        void updateInput();
        void simulate(double dt);
        void draw2D();
        void draw3D();

        AppConfig config_{};
        GLFWwindow* window_ = nullptr; // raw pointer owned by GLFW logic

        int fbWidth_ = 0;
        int fbHeight_ = 0;

        // Subsystems (smart pointers for automatic RAII cleanup)
        std::unique_ptr<core::Simulation> simulation_;
        std::unique_ptr<core::OrbitCamera> camera_;
        std::unique_ptr<render::Renderer2D> r2d_;
        std::unique_ptr<render::Renderer3D> r3d_;
        std::unique_ptr<ui::ToolbarState> toolbarState_;
        std::unique_ptr<InputState> input_;

        // Timing and input state
        double lastTime_ = 0.0;
        double scrollDelta_ = 0.0;
    };

}