#pragma once
#include "./../../../Common/Logger/Logger.h"
#include "./../../../Common/IO/IO.h"

namespace {
    int keyToChange = -1;  // Index of the keybind being edited (-1 means none)
    ImGuiID lastClickedCell = 0;  // Track the clicked keybind cell to manage focus

}
inline void renderKeyBindWindow(GLFWwindow* window) {

    // Begin a three-column layout
    ImGui::Columns(2, "Keybind Columns");

    // Set up the headers for the columns
    ImGui::Text("Description");
    ImGui::NextColumn();
    ImGui::Text("Current Keybind");
    ImGui::NextColumn();
    ImGui::Separator(); // Separate the header from the content

    // Example keybinds with dummy variables for each row
    std::map<std::string, std::string>& tempKeyBinds = IO::getBoundKeys();
    int currentRow = 0;

    for (const auto& pair : tempKeyBinds) {
        // Column 1: Keybind description
        ImGui::Text("%s", pair.first.c_str());
        ImGui::NextColumn();

        // Column 2: Current keybind (clickable)
        ImGui::PushID(currentRow); // Ensure each keybind has a unique ID

        ImGuiID currentCellID = ImGui::GetID((pair.second + "_cell").c_str());

        // Check if the current key is being edited
        if (keyToChange == currentRow) {
            ImGui::Text("Press any key combination...");
            IO::skipProcessing = true;
            static std::vector<int> capturedKeys;
            static bool anyKeyPressed = false;
            // Capture key presses for combinations
            for (int i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; ++i) {
                if (glfwGetKey(window, i) == GLFW_PRESS && std::find(capturedKeys.begin(), capturedKeys.end(), i) == capturedKeys.end()) {
                    capturedKeys.push_back(i); // Add the key to the captured combination
                    anyKeyPressed = true;
                }
            }

            // If all keys are released, save the combination
            if (anyKeyPressed && std::all_of(capturedKeys.begin(), capturedKeys.end(), [&](int key) { return glfwGetKey(window, key) == GLFW_RELEASE; })) {

                // Update the keybind in the map
                IO::updateKeyToCheckFor(pair.first, capturedKeys);
                // Exit key capture mode
                capturedKeys.clear();
                keyToChange = -1;
                lastClickedCell = 0;
                IO::skipProcessing = false;
                anyKeyPressed = false;
            }
        }
        else {
            // Display the current keybind normally, clickable
            ImGui::Text("%s", pair.second.c_str());

            // Detect if the keybind cell is clicked
            if (ImGui::IsItemClicked()) {
                keyToChange = currentRow;  // Set this row to key capture mode
                lastClickedCell = currentCellID;  // Track the clicked cell
                IO::skipProcessing = true;
            }
        }

        ImGui::PopID();  // Pop the unique ID
        ImGui::NextColumn();

        ImGui::Separator();  // Separate the individual keybinds for readability
        currentRow++;
    }

    ImGui::Columns(1);  // End the column layout
}