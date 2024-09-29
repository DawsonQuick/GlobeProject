#pragma once

namespace {
    auto renderTextWithColor = [](const char* text, ImVec4 color) {
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Text(text);
        ImGui::PopStyleColor();
        };
}


inline void renderGuiLogger(int display_h , int display_w) {


    //-------------------------------------------------------------------------------------------------------------------------------------------------
    //                                                  Application Log Gui window 
    //-------------------------------------------------------------------------------------------------------------------------------------------------
    if (Logger::isDisplayImGuiLogger()) {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));  // RGBA: Black with 50% opacity
        ImGui::SetNextWindowPos(ImVec2(0, display_h * 0.014), ImGuiCond_Always);
        ImGui::Begin("Log Window", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
       
        //-----------------------------------------------------------------------------------------
        // LogWindow Control area
        // ----------------------------------------------------------------------------------------
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
        //---------------------------------------------------------------------------------------

        // Create a scrolling region
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar)) {
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