#pragma once

#include "core/simulation.h"

#include <imgui.h>

namespace ui {

    /**
     * @brief Persistent UI state for the toolbar inputs.
     */
    struct ToolbarState {
        int colsInput = 50;
        int rowsInput = 50;
    };

    /**
     * @brief Actions emitted by the toolbar during this frame.
     */
    struct ToolbarActions {
        bool toggledRun = false;
        bool requestStep = false;
        bool requestClear = false;
        int resizeCols = -1;    // -1 for unchanged
        int resizeRows = -1;    // -1 for unchanged
        float newSpeed = -1.0f; // -1 for unchanged
    };

    /**
     * @brief Draw the toolbar and return actions requested by the user.
     * @param state Persistent widget state (inputs).
     * @param sim   Simulation to read current values.
     */
    ToolbarActions drawToolbar(ToolbarState& state, const core::Simulation& sim);

}
