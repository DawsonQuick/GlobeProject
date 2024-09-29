#pragma once
#include "./../../Common/Vendor/imgui/imgui.h"
#include "./../../Common/Camera/Camera.h"
#include "./../../Common/Logger/Logger.h"
#include "./../Scene/Scene.h"
#include "Menu/Menu.h"
#include "GuiLogger/GuiLogger.h"

inline void renderImGui(GLFWwindow* window) {
    
    renderMenu(window);
    
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);

    /*
    // Set ImGui window position and size
    ImGui::SetNextWindowPos(ImVec2(display_w * 0.85f, display_h*0.014), ImGuiCond_Always); // Set position to right side
    ImGui::SetNextWindowSize(ImVec2(display_w * 0.15f, display_h), ImGuiCond_Always); // Set width to 25% of window
    float tmp = cam.getCameraRadius();
    ImGui::Begin("Right Content", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);  
    if (ImGui::TreeNode("Camera Properties")) {
        if (ImGui::DragFloat("My Float", &tmp, 1000.0f)) {
            // Set the new value if it has changed
            cam.setCameraRadius(tmp);
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Scene Entities")) {
        for (const auto& pair : Scene::getInstance().getEntites()) {
            ImGui::Text("Entity Id: %.d", static_cast<uint32_t>(pair.first));
        }
        ImGui::TreePop();
    }
    ImGui::End();
    */

    renderGuiLogger(display_h, display_w);

}
