#pragma once

#include "core/simulation.h"

#include <glad/glad.h>

namespace render {

    /**
     * @brief Convert window mouse coordinates to a grid cell in a given viewport.
     * @param mx Mouse X in window coordinates.
     * @param my Mouse Y in window coordinates.
     * @param vpX Viewport origin X in window coordinates.
     * @param vpY Viewport origin Y in window coordinates.
     * @param vpW Viewport width in pixels.
     * @param vpH Viewport height in pixels.
     * @param gridW Grid width in cells.
     * @param gridH Grid height in cells.
     * @param cx Output cell X or -1 if outside.
     * @param cy Output cell Y or -1 if outside.
     */
    void mouseToCell(double mx, double my, int vpX, int vpY, int vpW, int vpH, int gridW, int gridH, int& cx, int& cy);

    /**
     * @brief Fullscreen 2D renderer for the state texture and grid overlay.
     */
    class Renderer2D {
    public:
        explicit Renderer2D(core::Simulation& sim);
        ~Renderer2D();

        /**
         * @brief Draw the 2D grid into the active framebuffer.
         * @param viewportW Viewport width in pixels.
         * @param viewportH Viewport height in pixels.
         * @param hoverX Hovered cell X, or -1 if none.
         * @param hoverY Hovered cell Y, or -1 if none.
         */
        void draw(int viewportW, int viewportH, int hoverX, int hoverY);

    private:
        core::Simulation& sim_;
        GLuint program_ = 0;
        GLuint vao_ = 0;

        // Uniform locations
        GLint uGridSize_ = -1;
        GLint uViewportPx_ = -1;
        GLint uState_ = -1;
        GLint uDeadColor_ = -1;
        GLint uAliveColor_ = -1;
        GLint uLineThicknessPx_ = -1;
        GLint uLineColor_ = -1;
        GLint uHoverCell_ = -1;
        GLint uHoverBoost_ = -1;
        GLint uEdgeUColor_ = -1;
        GLint uEdgeVColor_ = -1;
        GLint uEdgeThicknessPx_ = -1;
    };

}
