#include "../../include/core/gameLogic.h"

#include <algorithm>

namespace core {

    Life::Life(int width, int height)
        : gridWidth_(width)
        , gridHeight_(height)
        , currentBuffer_(static_cast<size_t>(width)* height, 0)
        , nextBuffer_(static_cast<size_t>(width)* height, 0) {
    }

    void Life::clear() {
        std::fill(currentBuffer_.begin(), currentBuffer_.end(), 0);
    }

    void Life::step() {
        for (int y = 0; y < gridHeight_; ++y) {
            for (int x = 0; x < gridWidth_; ++x) {
                const int xm = wrap(x - 1, gridWidth_);
                const int xp = wrap(x + 1, gridWidth_);
                const int ym = wrap(y - 1, gridHeight_);
                const int yp = wrap(y + 1, gridHeight_);

                int aliveNeighbours = 0;
                aliveNeighbours += currentBuffer_[ym * gridWidth_ + xm];
                aliveNeighbours += currentBuffer_[ym * gridWidth_ + x];
                aliveNeighbours += currentBuffer_[ym * gridWidth_ + xp];
                aliveNeighbours += currentBuffer_[y * gridWidth_ + xm];
                aliveNeighbours += currentBuffer_[y * gridWidth_ + xp];
                aliveNeighbours += currentBuffer_[yp * gridWidth_ + xm];
                aliveNeighbours += currentBuffer_[yp * gridWidth_ + x];
                aliveNeighbours += currentBuffer_[yp * gridWidth_ + xp];

                const uint8_t alive = currentBuffer_[y * gridWidth_ + x] ? 1u : 0u;

                // Conway's rules
                nextBuffer_[y * gridWidth_ + x] = alive ? (aliveNeighbours == 2 || aliveNeighbours == 3) : (aliveNeighbours == 3);
            }
        }

        std::swap(currentBuffer_, nextBuffer_);
    }

    uint8_t& Life::at(int x, int y) {
        return currentBuffer_[y * gridWidth_ + x];
    }

    const uint8_t* Life::data() const {
        return currentBuffer_.data();
    }

    int Life::wrap(int index, int length) const {
        if (index >= length) index -= length;
        else if (index < 0)  index += length;
        return index;
    }

}