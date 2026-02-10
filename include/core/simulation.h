#pragma once

#include "gameLogic.h"

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

        // Prevent copying (simulation owns unique GL resources)
        Simulation(const Simulation&) = delete;
        Simulation& operator=(const Simulation&) = delete;

        /**
         * @brief Current grid width in cells.
         */
        int width() const;

        /**
         * @brief Current grid height in cells.
         */
        int height() const;

        /**
         * @brief OpenGL texture handle containing the state (GL_R8).
         */
        GLuint stateTexture() const;

        /**
         * @brief Set fixed-step simulation frequency.
         * @param sps Steps per second (> 0).
         */
        void setStepsPerSecond(float sps);

        /**
         * @brief Get current steps per second.
         */
        float stepsPerSecond() const;

        /**
         * @brief Toggle running/paused state.
         */
        void toggleRun();

        /**
         * @brief True if the simulation is running.
         */
        bool isRunning() const;

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
        void uploadAll();
        void uploadCell(int x, int y);

        Life life_;          // CPU-side state engine
        int width_ = 0;      // Cached width
        int height_ = 0;     // Cached height
        GLuint tex_ = 0;     // GL texture mirroring state (GL_R8)

        bool running_ = false;
        float stepsPerSec_ = 5.0f;
        double accumulator_ = 0.0;
    };

}