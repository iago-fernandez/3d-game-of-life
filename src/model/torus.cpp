#include "../../include/model/torus.h"

#include <vector>
#include <cmath>
#include <glm/gtc/constants.hpp>

namespace model {

    struct Vertex {
        float position[3];
        float normal[3];   // currently not used (only ambient ilumination in the shader)
        float uv[2];
    };

    TorusMesh makeTorusGrid(int columns, int rows, float outerRadius, float innerRadius) {
        const int vertexCols = columns + 1;
        const int vertexRows = rows + 1;
        constexpr float TWO_PI = glm::two_pi<float>();

        std::vector<Vertex> vertices(static_cast<std::size_t>(vertexCols) * static_cast<std::size_t>(vertexRows));
        auto vertexIndex = [&](int i, int j) -> int { return j * vertexCols + i; };

        // Generate vertices
        for (int j = 0; j < vertexRows; ++j) {
            const float v = static_cast<float>(j) / static_cast<float>(rows);
            const float angMinor = (0.5f - v) * TWO_PI;  // minor angle (tube)
            const float cosMinor = std::cos(angMinor);
            const float sinMinor = std::sin(angMinor);

            for (int i = 0; i < vertexCols; ++i) {
                const float u = static_cast<float>(i) / static_cast<float>(columns);
                const float angMajor = u * TWO_PI;        // major angle (ring)
                const float cosMajor = std::cos(angMajor);
                const float sinMajor = std::sin(angMajor);

                const float ring = outerRadius + innerRadius * cosMinor;

                Vertex vertex{};
                vertex.position[0] = ring * cosMajor;
                vertex.position[1] = ring * sinMajor;
                vertex.position[2] = innerRadius * sinMinor;

                vertex.normal[0] = cosMajor * cosMinor;
                vertex.normal[1] = sinMajor * cosMinor;
                vertex.normal[2] = sinMinor;

                vertex.uv[0] = 1.0f - u;  // mirror U so grid-right maps to torus-right
                vertex.uv[1] = v;

                vertices[static_cast<std::size_t>(vertexIndex(i, j))] = vertex;
            }
        }

        // Build triangle indices (two triangles per quad)
        std::vector<unsigned int> indices;
        indices.reserve(static_cast<std::size_t>(columns) * static_cast<std::size_t>(rows) * 6);
        for (int j = 0; j < rows; ++j) {
            for (int i = 0; i < columns; ++i) {
                const unsigned int i0 = static_cast<unsigned int>(vertexIndex(i, j));
                const unsigned int i1 = static_cast<unsigned int>(vertexIndex(i + 1, j));
                const unsigned int i2 = static_cast<unsigned int>(vertexIndex(i, j + 1));
                const unsigned int i3 = static_cast<unsigned int>(vertexIndex(i + 1, j + 1));

                indices.push_back(i0);
                indices.push_back(i2);
                indices.push_back(i1);

                indices.push_back(i1);
                indices.push_back(i2);
                indices.push_back(i3);
            }
        }

        // Upload to GPU
        TorusMesh mesh{};
        glGenVertexArrays(1, &mesh.vao_);
        glGenBuffers(1, &mesh.vbo_);
        glGenBuffers(1, &mesh.ebo_);

        glBindVertexArray(mesh.vao_);

        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo_);
        glBufferData(GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
            vertices.data(),
            GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
            indices.data(),
            GL_STATIC_DRAW);

        const GLsizei stride = static_cast<GLsizei>(sizeof(Vertex));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, uv));

        glBindVertexArray(0);

        mesh.indexCount_ = static_cast<GLsizei>(indices.size());
        return mesh;
    }

    void destroyTorus(TorusMesh& mesh) {
        if (mesh.ebo_) glDeleteBuffers(1, &mesh.ebo_);
        if (mesh.vbo_) glDeleteBuffers(1, &mesh.vbo_);
        if (mesh.vao_) glDeleteVertexArrays(1, &mesh.vao_);
        mesh = {}; // reset handles to 0
    }

}