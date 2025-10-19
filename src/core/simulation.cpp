#include "../../include/core/simulation.h"

#include <algorithm>

namespace core {

    Simulation::Simulation(int w, int h) : life_(w, h), width_(w), height_(h) {
        glGenTextures(1, &tex_);
        glBindTexture(GL_TEXTURE_2D, tex_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width_, height_, 0, GL_RED, GL_UNSIGNED_BYTE, life_.data());
    }

    Simulation::~Simulation() {
        if (tex_) glDeleteTextures(1, &tex_);
    }

    void Simulation::setStepsPerSecond(float sps) {
        stepsPerSec_ = (sps <= 0.0f) ? 0.0001f : sps;
    }

    void Simulation::advance(double dt) {
        if (!running_) return;
        accumulator_ += dt;
        const double period = 1.0 / std::max(0.0001, (double)stepsPerSec_);
        int steps = 0;
        while (accumulator_ >= period && steps < 240) { // prevents "spiral of death" (no drawing if there are more than 240 steps per frame)
            life_.step();
            uploadAll();
            accumulator_ -= period;
            ++steps;
        }
    }

    void Simulation::stepOnce() {
        life_.step();
        uploadAll();
    }

    void Simulation::clear() {
        life_.clear();
        uploadAll();
    }

    void Simulation::toggleCell(int x, int y) {
        if (x < 0 || y < 0 || x >= width_ || y >= height_) return;
        uint8_t& v = life_.at(x, y);
        v ^= 1;
        uploadCell(x, y);
    }

    void Simulation::resize(int newW, int newH) {
        Life old = life_;
        int oldW = width_;
        int oldH = height_;

        width_ = newW;
        height_ = newH;
        life_ = Life(width_, height_);

        int copyW = std::min(oldW, width_);
        int copyH = std::min(oldH, height_);
        int srcY0 = oldH - copyH;      // source bottom offset
        int dstY0 = height_ - copyH;   // destination bottom offset
        for (int y = 0; y < copyH; ++y) {
            for (int x = 0; x < copyW; ++x) {
                life_.at(x, dstY0 + y) = old.at(x, srcY0 + y);
            }
        }

        glBindTexture(GL_TEXTURE_2D, tex_);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width_, height_, 0, GL_RED, GL_UNSIGNED_BYTE, life_.data());
        accumulator_ = 0.0;
    }

    void Simulation::uploadAll() {
        glBindTexture(GL_TEXTURE_2D, tex_);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, GL_RED, GL_UNSIGNED_BYTE, life_.data());
    }

    void Simulation::uploadCell(int x, int y) {
        glBindTexture(GL_TEXTURE_2D, tex_);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &life_.at(x, y));
    }

}
