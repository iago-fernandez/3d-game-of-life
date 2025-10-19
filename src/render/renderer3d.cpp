#include "../../include/render/renderer3d.h"

#include "../../include/utils/shaderUtils.h"

#include <glm/gtc/matrix_transform.hpp>

namespace render {

    Renderer3D::Renderer3D(core::Simulation& sim) : sim_(sim) {
        program_ = makeProgramFromFiles(SHADER_DIR "/shader3d.vert", SHADER_DIR "/shader3d.frag");
        glUseProgram(program_);

        uMVP_ = glGetUniformLocation(program_, "uMVP");
        uState_ = glGetUniformLocation(program_, "uState");
        uDeadColor_ = glGetUniformLocation(program_, "uDeadColor");
        uAliveColor_ = glGetUniformLocation(program_, "uAliveColor");
        uGridSize_ = glGetUniformLocation(program_, "uGridSize");
        uLinePx_ = glGetUniformLocation(program_, "uLinePx");
        uLineColor_ = glGetUniformLocation(program_, "uLineColor");
        uEdgeUColor_ = glGetUniformLocation(program_, "uEdgeUColor");
        uEdgeVColor_ = glGetUniformLocation(program_, "uEdgeVColor");
        uEdgePxUV_ = glGetUniformLocation(program_, "uEdgePxUV");

        torus_ = model::makeTorusGrid(sim_.width(), sim_.height(), 2.0f, 0.7f);
    }

    Renderer3D::~Renderer3D() {
        destroyTorus(torus_);
        if (program_) glDeleteProgram(program_);
    }

    void Renderer3D::rebuildMesh(int cols, int rows) {
        destroyTorus(torus_);
        torus_ = model::makeTorusGrid(cols, rows, 2.0f, 0.7f);
    }

    void Renderer3D::draw(const core::OrbitCamera& cam, int viewportW, int viewportH) {
        float aspect = (viewportW > 0) ? (float)viewportW / (float)viewportH : 1.0f;

        glm::mat4 proj = glm::perspective(glm::radians(25.0f), aspect, 0.1f, 200.0f);
        glm::mat4 view = cam.view();
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 mvp = proj * view * model;

        glUseProgram(program_);

        glUniformMatrix4fv(uMVP_, 1, GL_FALSE, &mvp[0][0]);
        glUniform1i(uState_, 0);
        glUniform2i(uGridSize_, sim_.width(), sim_.height());
        glUniform3f(uDeadColor_, 1.0f, 1.0f, 1.0f);
        glUniform3f(uAliveColor_, 0.0f, 0.0f, 0.0f);
        glUniform1f(uLinePx_, 0.7f);
        glUniform3f(uLineColor_, 0.75f, 0.75f, 0.75f);
        glUniform3f(uEdgeUColor_, 0.30f, 0.50f, 1.00f);
        glUniform3f(uEdgeVColor_, 1.00f, 0.35f, 0.35f);
        glUniform1f(uEdgePxUV_, 2.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sim_.stateTexture());

        glBindVertexArray(torus_.vao_);
        glDrawElements(GL_TRIANGLES, torus_.indexCount_, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }

}
