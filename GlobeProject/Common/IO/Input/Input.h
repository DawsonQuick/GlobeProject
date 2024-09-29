#pragma once
#pragma optimize("", off)
#include "./../sharedIOData.h"
#include "./../../../Common/Vendor/imgui/imgui.h"

#include <string>
#include <map>
#include <algorithm>

namespace IO {
    static bool skipProcessing = false;
	void addKeyToCheckFor(std::string name, std::vector<int> keyIds, std::function<void()> action, KeyDetectionAction detectionAction, InputType inputType, bool overrideImGuiFocused);
	void processWindowUpdate(GLFWwindow* window, float currentFrame);
	void updateKeyToCheckFor(std::string name, std::vector<int> keyIds);
	std::map<std::string, std::string>& getBoundKeys();

}
#pragma optimize("", on)