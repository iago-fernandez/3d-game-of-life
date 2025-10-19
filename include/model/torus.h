#pragma once

#include <glad/glad.h>

namespace model {

    /**
     * @brief GPU buffers and counts for a torus mesh.
     */
    struct TorusMesh {
        GLuint vao_ = 0;
        GLuint vbo_ = 0;
        GLuint ebo_ = 0;
        GLsizei indexCount_ = 0;
    };

    /**
     * @brief Build a torus grid mesh and upload it to GPU buffers.
     * @param columns Number of grid columns along the major ring.
     * @param rows Number of grid rows along the minor tube.
     * @param outerRadius Distance from origin to the center of the tube.
     * @param innerRadius Tube radius.
     * @return TorusMesh with VAO/VBO/EBO initialized and ready to draw with GL_TRIANGLES.
     */
    TorusMesh makeTorusGrid(int columns, int rows, float outerRadius, float innerRadius);

    /**
     * @brief Destroy GPU resources held by a TorusMesh and reset it to defaults.
     * @param t Mesh to destroy.
     */
    void destroyTorus(TorusMesh& t);

}