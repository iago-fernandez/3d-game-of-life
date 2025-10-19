#pragma once

#include "core/gameLogic.h"

#include <glad/glad.h>

namespace core {

    /**
     * @brief Simulation wrapper around Life and its GPU state texture.
     *
     * Owns the CPU grid (Life) and a GL R8 texture that mirrors the state for rendering.
     * Provides timing control (steps per second) and utility operations.
     */
    class Simulation {
    public:
        /**
         * @brief Create a simulation with an initial grid size.
         * @param w Number of columns.
         * @param h Number of rows.
         */
        Simulation(int w, int h);
        ~Simulation();

        /**
         * @brief Current grid width in cells.
         */
        int width() const {
            return width_;
        }

        /**
         * @brief Current grid height in cells.
         */
        int height() const {
            return height_;
        }

        /**
         * @brief OpenGL texture handle containing the state (GL_R8).
         */
        GLuint stateTexture() const {
            return tex_;
        }

        /**
         * @brief Set fixed-step simulation frequency.
         * @param sps Steps per second (> 0).
         */
        void setStepsPerSecond(float sps);

        /**
         * @brief Get current steps per second.
         */
        float stepsPerSecond() const {
            return stepsPerSec_;
        }

        /**
         * @brief Toggle running/paused state.
         */
        void toggleRun() {
            running_ = !running_;
        }

        /**
         * @brief True if the simulation is running.
         */
        bool isRunning() const {
            return running_;
        }

        /**
         * @brief Advance the simulation according to elapsed time.
         * @param dt Delta time in seconds since last frame.
         */
        void advance(double dt);

        /**
         * @brief Execute one step immediately.
         */
        void stepOnce();

        /**
         * @brief Clear all cells to dead.
         */
        void clear();

        /**
         * @brief Toggle a single cell.
         * @param x Column index.
         * @param y Row index.
         */
        void toggleCell(int x, int y);

        /**
         * @brief Resize the grid, preserving the bottom rows/left columns overlap.
         * @param newW New number of columns.
         * @param newH New number of rows.
         */
        void resize(int newW, int newH);

    private:
        // Upload entire CPU buffer to the GL texture
        void uploadAll();

        // Upload a single cell to the GL texture
        void uploadCell(int x, int y);

    private:
        Life life_;                  // cpu-side state
        int width_ = 0;              // number of columns
        int height_ = 0;             // number of rows
        GLuint tex_ = 0;             // gl texture containing the state (GL_R8)

        bool running_ = false;       // play/pause flag
        float stepsPerSec_ = 5.0f;   // fixed step frequency
        double accumulator_ = 0.0;   // accumulator for fixed stepping
    };

}
