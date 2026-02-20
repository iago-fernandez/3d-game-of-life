/**
 * @file input.h
 * @brief Defines the per-frame input snapshot data structures for mouse and keyboard state.
 */
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

}