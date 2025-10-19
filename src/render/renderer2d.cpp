#include "../../include/render/renderer2d.h"

#include "../../include/utils/shaderUtils.h"

namespace render {

    void mouseToCell(double mx, double my, int vpX, int vpY, int vpW, int vpH, int gridW, int gridH, int& cx, int& cy) {
        double lx = mx - (double)vpX;
        double ly = my - (double)vpY;

        if (lx < 0.0 || ly < 0.0 || lx >= vpW || ly >= vpH) {
            cx = cy = -1;
            return;
        }

        cx = (int)(lx * gridW / (double)vpW);
        cy = (int)(((double)vpH - 1.0 - ly) * gridH / (double)vpH);

        if (cx < 0 || cy < 0 || cx >= gridW || cy >= gridH) {
            cx = cy = -1;
        }
    }

    Renderer2D::Renderer2D(core::Simulation& sim) : sim_(sim) {
        program_ = makeProgramFromFiles(SHADER_DIR "/shader2d.vert", SHADER_DIR "/shader2d.frag");
        glUseProgram(program_);

        uGridSize_ = glGetUniformLocation(program_, "uGridSize");
        uViewportPx_ = glGetUniformLocation(program_, "uViewportPx");
        uState_ = glGetUniformLocation(program_, "uState");
        uDeadColor_ = glGetUniformLocation(program_, "uDeadColor");
        uAliveColor_ = glGetUniformLocation(program_, "uAliveColor");
        uLineThicknessPx_ = glGetUniformLocation(program_, "uLineThicknessPx");
        uLineColor_ = glGetUniformLocation(program_, "uLineColor");
        uHoverCell_ = glGetUniformLocation(program_, "uHoverCell");
        uHoverBoost_ = glGetUniformLocation(program_, "uHoverBoost");
        uEdgeUColor_ = glGetUniformLocation(program_, "uEdgeUColor");
        uEdgeVColor_ = glGetUniformLocation(program_, "uEdgeVColor");
        uEdgeThicknessPx_ = glGetUniformLocation(program_, "uEdgeThicknessPx");

        glGenVertexArrays(1, &vao_);
    }

    Renderer2D::~Renderer2D() {
        if (vao_) glDeleteVertexArrays(1, &vao_);
        if (program_) glDeleteProgram(program_);
    }

    void Renderer2D::draw(int viewportW, int viewportH, int hoverX, int hoverY) {
        glUseProgram(program_);

        glUniform2f(uViewportPx_, (float)viewportW, (float)viewportH);
        glUniform2i(uGridSize_, sim_.width(), sim_.height());
        glUniform3f(uDeadColor_, 0.92f, 0.92f, 0.92f);
        glUniform3f(uAliveColor_, 0.12f, 0.12f, 0.12f);
        glUniform1f(uLineThicknessPx_, 0.5f);
        glUniform3f(uLineColor_, 0.76f, 0.76f, 0.76f);
        glUniform1f(uHoverBoost_, 0.25f);
        glUniform1i(uState_, 0);
        glUniform2i(uHoverCell_, hoverX, hoverY);
        glUniform3f(uEdgeUColor_, 0.30f, 0.50f, 1.00f);
        glUniform3f(uEdgeVColor_, 1.00f, 0.35f, 0.35f);
        glUniform1f(uEdgeThicknessPx_, 1.5f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sim_.stateTexture());

        glBindVertexArray(vao_);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

}
