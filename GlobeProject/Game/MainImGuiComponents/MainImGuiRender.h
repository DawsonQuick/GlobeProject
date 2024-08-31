#pragma once
#include "./../../Common/Vendor/imgui/imgui.h"
#include "./../../Common/Camera/Camera.h"
#include "./../../Common/Logger/Logger.h"

namespace {
    auto renderTextWithColor = [](const char* text, ImVec4 color) {
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Text(text);
        ImGui::PopStyleColor();
        };
}

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


    //-------------------------------------------------------------------------------------------------------------------------------------------------
    //                                                  Application Log Gui window 
    //-------------------------------------------------------------------------------------------------------------------------------------------------
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));  // RGBA: Black with 50% opacity
    ImGui::SetNextWindowPos(ImVec2(0, display_h * 0.014), ImGuiCond_Always);
    if (cam.getCameraInfo().isLogToggled) {
        ImGui::Begin("Log Window" , nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar );
        // Create a separate child window for controls
        ImGui::BeginChild("ControlsRegion", ImVec2(0, 100), true); // Height of 100 pixels
        ImGui::Columns(4, "PlayerInfoColumns", false); // 2 columns, don't auto-fit columns
        static int selected = 1; 
        for (int n = 0; n < 4; n++)
        {
            ImGui::SetColumnWidth(n, display_w / 4);
            char buf[32];
            sprintf_s(buf, Logger::getStringLogLevel(n).c_str());
            ImGui::PushID(n); // Ensure each selectable has a unique ID
            if (ImGui::Selectable(buf, selected == n))
            {
                selected = n;
                Logger::setActiveLogLevel(static_cast<LogLevel>(n));
            }
            ImGui::PopID(); // Restore the ID stack
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
        ImGui::Columns(2, nullptr, false); // 2 columns, no border
        // Set the column width for the label
        ImGui::SetColumnWidth(0, ImGui::GetWindowSize()[0] * 0.06);
        // Show the label in the first column
        ImGui::Text("%s", "Log History Cap");
        // Move to the second column
        ImGui::NextColumn();
        // Show the slider in the second column
        ImGui::PushItemWidth(ImGui::GetWindowSize()[0] * 0.93);
        if (ImGui::SliderInt("", &Logger::currentCapacity, 0, 1000)) {
            Logger::updateLogCapacity();
        }
        ImGui::PopItemWidth();
        // End the columns
        ImGui::Columns(1);
        ImGui::EndChild();
        // Create a scrolling region
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar)) {
            // Display each log entry
            for (const auto& log : Logger::getLogger()) {
                const char* string = log.c_str() + 1;
                switch (log.c_str()[0]) {
                case 'D': //Debug
                    renderTextWithColor(string, ImVec4(0.0, 0.2, 1.0, 1.0)); //Blue
                    break;
                case 'I': //Info
                    renderTextWithColor(string, ImVec4(1.0, 1.0, 1.0, 1.0)); //White
                    break;
                case 'W': //Warn
                    renderTextWithColor(string, ImVec4(1.0, 1.0, 0.0, 1.0)); //Yellow
                    break;
                case 'E': //Error
                    renderTextWithColor(string, ImVec4(1.0, 0.0, 0.0, 1.0)); //Red
                    break;
                default: //Default
                    ImGui::TextUnformatted(string); //Unformatted
                    break;
                }
            }
            // Auto-scroll to the bottom if we are at the bottom already
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHere(1.0f);
            }
        }
        ImGui::EndChild();
    ImGui::End();
        ImGui::PopStyleColor();  // Restore the original background color
    }
    //-------------------------------------------------------------------------------------------------------------------------------------------------
  

}
