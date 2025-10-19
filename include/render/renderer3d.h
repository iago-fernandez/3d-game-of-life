#pragma once

#include "core/simulation.h"
#include "core/camera.h"
#include "model/torus.h"

#include <glad/glad.h>
#include <glm/mat4x4.hpp>

namespace render {

    /**
     * @brief 3D renderer for the torus grid.
     */
    class Renderer3D {
    public:
        explicit Renderer3D(core::Simulation& sim);
        ~Renderer3D();

        /**
         * @brief Draw the torus in the given viewport using the provided camera.
         * @param cam Orbit camera for view transform.
         * @param viewportW Viewport width in pixels.
         * @param viewportH Viewport height in pixels.
         */
        void draw(const core::OrbitCamera& cam, int viewportW, int viewportH);

        /**
         * @brief Rebuild the torus mesh when grid size changes.
         * @param cols New grid columns.
         * @param rows New grid rows.
         */
        void rebuildMesh(int cols, int rows);

    private:
        core::Simulation& sim_;
        GLuint program_ = 0;
        model::TorusMesh torus_{};

        // Uniform locations
        GLint uMVP_ = -1;
        GLint uState_ = -1;
        GLint uGridSize_ = -1;
        GLint uDeadColor_ = -1;
        GLint uAliveColor_ = -1;
        GLint uLinePx_ = -1;
        GLint uLineColor_ = -1;
        GLint uEdgeUColor_ = -1;
        GLint uEdgeVColor_ = -1;
        GLint uEdgePxUV_ = -1;
    };

}
