#pragma once

namespace app {

    /**
     * @brief Per-frame input snapshot for mouse and keyboard state.
     */
    struct InputState {
        double mouseX_ = 0.0;
        double mouseY_ = 0.0;
        bool mouseL_ = false;

        bool wantCaptureMouse_ = false; // imgui requests mouse capture
    };

    /**
     * @brief Check whether a point is inside a rectangle.
     * @param mx Mouse X in window coordinates.
     * @param my Mouse Y in window coordinates.
     * @param x Rectangle top-left X.
     * @param y Rectangle top-left Y.
     * @param w Rectangle width.
     * @param h Rectangle height.
     * @return True if the point is inside the rectangle, false otherwise.
     */
    bool mouseInRect(double mx, double my, int x, int y, int w, int h);

}
