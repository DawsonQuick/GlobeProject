#pragma once

inline void renderGui(glm::vec3& lightPos) {
	ImGui::Begin("Right Content", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	if (ImGui::TreeNode("Terrain Chunk Properties")) {
		ImGui::DragFloat3("Light Position", glm::value_ptr(lightPos), 1000.0f);
		ImGui::TreePop();
	}
	ImGui::End();

}