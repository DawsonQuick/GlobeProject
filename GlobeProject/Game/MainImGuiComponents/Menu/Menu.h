#pragma once
#include "./../KeyBindComponentWindow/KeyBindComponentWindow.h"
#include "./../../Scene/SceneGui/SceneGui.h"

namespace {
    static bool showKeyBindWindow = false;
    static bool showSceneWindow = false;
}

inline void setWindowFullscreen(GLFWwindow* window) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    // Set the window to fullscreen
    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
}

inline void renderMenu(GLFWwindow* window) {

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit", "Ctrl+Q")) { glfwSetWindowShouldClose(window, GLFW_TRUE); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings")) {
            if (ImGui::MenuItem("KeyBinds")) { showKeyBindWindow = true; }
            if (ImGui::MenuItem("Scene Settings")) { showSceneWindow = true; }
            if (ImGui::MenuItem("FullScreen")) { setWindowFullscreen(window); }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (showKeyBindWindow) {
        ImGui::Begin("Key Bindings", &showKeyBindWindow);  // `&showOpenWindow` will close the window when the user clicks the close button
        renderKeyBindWindow(window);
        ImGui::End();
    }

    if (showSceneWindow) {
        ImGui::Begin("Scene Window", &showSceneWindow , ImGuiWindowFlags_MenuBar);  // `&showSceneWindow` will close the window when the user clicks the close button
        renderSceneGui(window);
        ImGui::End();
    }


}