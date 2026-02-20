/**
 * @file toolbar.cpp
 * @brief Implements the ImGui widget drawing logic and event handling for the simulation control panel.
 */
#include "../../include/ui/toolbar.h"

namespace ui {

    namespace {
        int clamp10_100(int v) {
            return v < 10 ? 10 : (v > 100 ? 100 : v);
        }
    }

    ToolbarActions drawToolbar(ToolbarState& s, const core::Simulation& sim) {
        ToolbarActions out{};

        const auto& io = ImGui::GetIO();
        const float screenW = io.DisplaySize.x;
        const float screenH = io.DisplaySize.y;
        const float margin = 12.0f;

        // Position the window at the bottom-right
        ImGui::SetNextWindowBgAlpha(0.45f);
        ImGui::SetNextWindowPos(
            ImVec2(screenW - margin, screenH - margin),
            ImGuiCond_Always,
            ImVec2(1.0f, 1.0f) // Pivot: Bottom-Right
        );

        ImGui::Begin("Toolbar", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_AlwaysAutoResize
        );

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 12));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

        float h = ImGui::GetFrameHeight();

        // Play / Pause
        if (ImGui::Button(sim.isRunning() ? "Pause" : "Play", ImVec2(64.0f, h))) {
            out.toggledRun = true;
        }
        ImGui::SameLine();

        // Step
        if (ImGui::Button("Step", ImVec2(64.0f, h))) {
            out.requestStep = true;
        }
        ImGui::SameLine();

        // Clear
        if (ImGui::Button("Clear", ImVec2(64.0f, h))) {
            out.requestClear = true;
        }
        ImGui::SameLine();

        // Speed Slider
        float speed = sim.stepsPerSecond();
        ImGui::SetNextItemWidth(180.0f);
        if (ImGui::SliderFloat("##Speed", &speed, 0.5f, 10.0f, "Speed: %.1f")) {
            out.newSpeed = speed;
        }

        // Separator
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(3.0f, h));
        ImGui::SameLine();

        // Grid Size Inputs
        const ImGuiInputTextFlags numFlags = ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_AutoSelectAll;
        const float threeChars = ImGui::CalcTextSize("000").x;
        const float inputWidth = threeChars + ImGui::GetStyle().FramePadding.x * 2.5f;

        bool commitRows = false;
        bool commitCols = false;

        // Rows Input
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Rows:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputInt("##Rows", &s.rowsInput, 0, 0, numFlags);

        if ((ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGuiKey_Enter)) || ImGui::IsItemDeactivatedAfterEdit()) {
            commitRows = true;
        }

        ImGui::SameLine();
        if (ImGui::Button("-##rows", ImVec2(0.0f, h))) {
            s.rowsInput = clamp10_100(s.rowsInput - 1);
            commitRows = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("+##rows", ImVec2(0.0f, h))) {
            s.rowsInput = clamp10_100(s.rowsInput + 1);
            commitRows = true;
        }
        ImGui::SameLine();

        // Columns Input
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Columns:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputInt("##Cols", &s.colsInput, 0, 0, numFlags);

        if ((ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGuiKey_Enter)) || ImGui::IsItemDeactivatedAfterEdit()) {
            commitCols = true;
        }

        ImGui::SameLine();
        if (ImGui::Button("-##cols", ImVec2(0.0f, h))) {
            s.colsInput = clamp10_100(s.colsInput - 1);
            commitCols = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("+##cols", ImVec2(0.0f, h))) {
            s.colsInput = clamp10_100(s.colsInput + 1);
            commitCols = true;
        }

        // Apply resize if requested
        if (commitRows || commitCols) {
            const int newRows = clamp10_100(s.rowsInput);
            const int newCols = clamp10_100(s.colsInput);

            out.resizeRows = newRows;
            out.resizeCols = newCols;

            s.rowsInput = newRows;
            s.colsInput = newCols;
        }

        ImGui::PopStyleVar(2);
        ImGui::End();

        return out;
    }

}