#pragma once
#include "./../../Common/Vendor/imgui/imgui.h"
#include "./../../Common/Camera/Camera.h"
inline void renderImGui(GLFWwindow* window , Camera &cam) {
 
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "Ctrl+O")) { /* Do something */ }
            if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do something */ }
            if (ImGui::MenuItem("Exit", "Ctrl+Q")) { /* Do something */ }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) { /* Do something */ }
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) { /* Do something */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "Ctrl+X")) { /* Do something */ }
            if (ImGui::MenuItem("Copy", "Ctrl+C")) { /* Do something */ }
            if (ImGui::MenuItem("Paste", "Ctrl+V")) { /* Do something */ }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) { /* Do something */ }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);

    // Set ImGui window position and size
    ImGui::SetNextWindowPos(ImVec2(display_w * 0.85f, display_h*0.014), ImGuiCond_Always); // Set position to right side
    ImGui::SetNextWindowSize(ImVec2(display_w * 0.15f, display_h), ImGuiCond_Always); // Set width to 25% of window
    float tmp = cam.getCameraRadius();
    ImGui::Begin("Right Content", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);  
    if (ImGui::TreeNode("Camera Properties")) {
        if (ImGui::DragFloat("My Float", &tmp, 100.0f)) {
            // Set the new value if it has changed
            cam.setCameraRadius(tmp);
        }
        ImGui::TreePop();
    }
    ImGui::End();



  

}
