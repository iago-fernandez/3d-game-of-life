#pragma once

#include <vector>
#include <cstdint>

class Life {

    public:

        int gridWidth, gridHeight;

        Life(int width, int height);

        void clear();
        void step();

        inline uint8_t& at(int x, int y) { return currentBuffer_[y * gridWidth + x]; }
        inline const uint8_t* data() const { return currentBuffer_.data(); }

    private:

        std::vector<uint8_t> currentBuffer_, nextBuffer_; // grid double buffer (matrix as row-major vector)

        inline int wrap(int index, int length) const {

            // Toroid borderline (bounds can be only surpassed by 1 unit, so this seems more efficient than %)
            if (index >= length) index -= length;
            else if (index < 0) index += length;

            return index;
        }
};
