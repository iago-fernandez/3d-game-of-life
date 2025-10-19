#include "../../include/app/input.h"

namespace app {

    bool mouseInRect(double mx, double my, int x, int y, int w, int h) {
        return (mx >= x) && (mx < (x + w)) && (my >= y) && (my < (y + h));
    }

}
