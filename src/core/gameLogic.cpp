#include "../../include/core/gameLogic.h"

Life::Life(int width, int height) : gridWidth(width), gridHeight(height), currentBuffer_(gridWidth * gridHeight, 0), nextBuffer_(gridWidth * gridHeight, 0) {}

void Life::clear() {
    std::fill(currentBuffer_.begin(), currentBuffer_.end(), 0);
}

void Life::step() {
    
    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {

            int aliveNeighbours = 0;
            const int xm = wrap(x - 1, gridWidth), xp = wrap(x + 1, gridWidth);
            const int ym = wrap(y - 1, gridHeight), yp = wrap(y + 1, gridHeight);

            aliveNeighbours += currentBuffer_[ym * gridWidth + xm];
            aliveNeighbours += currentBuffer_[ym * gridWidth + x];
            aliveNeighbours += currentBuffer_[ym * gridWidth + xp];
            aliveNeighbours += currentBuffer_[y * gridWidth + xm];
            aliveNeighbours += currentBuffer_[y * gridWidth + xp];
            aliveNeighbours += currentBuffer_[yp * gridWidth + xm];
            aliveNeighbours += currentBuffer_[yp * gridWidth + x];
            aliveNeighbours += currentBuffer_[yp * gridWidth + xp];

            const uint8_t isCurrentCellAlive = currentBuffer_[y * gridWidth + x];
            
            // Conway rules
            nextBuffer_[y * gridWidth + x] = (isCurrentCellAlive ? (aliveNeighbours == 2 || aliveNeighbours == 3) : (aliveNeighbours == 3));
        }
    }

    std::swap(currentBuffer_, nextBuffer_);
}
