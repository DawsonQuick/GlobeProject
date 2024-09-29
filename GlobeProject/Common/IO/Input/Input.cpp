#pragma optimize("", off)
#include "Input.h"
#include "./../../Camera/Camera.h"

namespace {
	inline std::string getStringRepresentation(std::vector<int> keyIds) {
		std::string tempString;

		std::vector<std::string> modifiers;
		std::vector<std::string> normalKeys;

		for (int key : keyIds) {
			std::string keyName = IO::getKeyName(key);
			if (keyName == "LEFT_SHIFT" || keyName == "LEFT_CONTROL" || keyName == "LEFT_ALT") {
				modifiers.push_back(keyName);
			}
			else {
				normalKeys.push_back(keyName);
			}
		}

		std::sort(modifiers.begin(), modifiers.end(), [](const std::string& a, const std::string& b) {
			return a < b;
			});

		for (const std::string& mod : modifiers) {
			if (!tempString.empty()) { tempString += " + "; }
			tempString += mod;
		}
		for (const std::string& key : normalKeys) {
			if (!tempString.empty()) { tempString += " + "; }
			tempString += key;
		}

		return tempString;
	}
}

namespace IO {

	void processWindowUpdate(GLFWwindow* window, float currentFrame) {

		deltaTime = currentFrame - lastFrameTime;
		lastFrameTime = currentFrame;

		if (skipProcessing) { return; }

		for (auto it = keyProcessors.begin(); it != keyProcessors.end(); ++it) {

			//Check to see if the individual key detection can happen when focused on ImGui windows
			//If it cannot happen when focused on an ImGui window, and if the user is currently focused on an ImGui window skip processing
			if (!it->second.overrideImGuiFocused && ImGui::GetIO().WantCaptureMouse) {continue;}

			it->second.process(window);
		}

		IO::activeCamera->update(window, deltaTime);

	}

	void addKeyToCheckFor(std::string name, std::vector<int> keyIds, std::function<void()> action, KeyDetectionAction detectionAction, InputType inputType, bool overrideImGuiFocused) {
		keyProcessors[name] = (KeyProcessor(std::move(keyIds), std::move(action), detectionAction, inputType, overrideImGuiFocused));
		nameToKeyStringRepresentation[name] = getStringRepresentation(keyProcessors[name].inputIds);
	}

	void updateKeyToCheckFor(std::string name, std::vector<int> keyIds) {
		keyProcessors[name].inputIds = keyIds;
		nameToKeyStringRepresentation[name] = getStringRepresentation(keyProcessors[name].inputIds);
	}

	void removeKeyCheck(std::string name) {
		keyProcessors.erase(name);
	}

	std::map<std::string, std::string>& getBoundKeys() {
		return nameToKeyStringRepresentation;
	}


}
#pragma optimize("", on)