#pragma once

#include <vector>
#include <cstdint>

namespace core {

    /**
     * @brief Game of Life state and rules on a toroidal grid.
     *
     * Stores the current generation in a row-major byte buffer (0 = dead, 1 = alive)
     * and computes the next generation using double buffering.
     */
    class Life {
    public:
        /**
         * @brief Construct a grid of size width x height (all cells dead).
         * @param width Number of columns.
         * @param height Number of rows.
         */
        Life(int width, int height);

        /**
         * @brief Set all cells to dead.
         */
        void clear();

        /**
         * @brief Advance the simulation by one generation.
         *
         * Applies Conway's rules with wrap-around at the edges (toroidal topology).
         */
        void step();

        /**
         * @brief Access a cell by coordinates.
         * @param x Column index in [0, gridWidth).
         * @param y Row index in [0, gridHeight).
         * @return Reference to the cell value (0 or 1).
         */
        inline uint8_t& at(int x, int y) {
            return currentBuffer_[y * gridWidth_ + x];
        }

        /**
         * @brief Pointer to the raw buffer for GPU uploads or inspection.
         * @return Read-only pointer to the current buffer (row-major, size = width*height).
         */
        inline const uint8_t* data() const {
            return currentBuffer_.data();
        }

        int gridWidth_;   // number of columns
        int gridHeight_;  // number of rows

    private:
        std::vector<uint8_t> currentBuffer_; // current generation buffer (row-major)
        std::vector<uint8_t> nextBuffer_;    // next generation buffer (work buffer)

        /**
         * @brief Wrap an index to [0, length) with single-step overflow handling.
         * @param index Input index, possibly -1 or length.
         * @param length Interval length.
         * @return Wrapped index inside [0, length).
         */
        inline int wrap(int index, int length) const {
            if (index >= length) index -= length;
            else if (index < 0)  index += length;
            return index;
        }
    };

}