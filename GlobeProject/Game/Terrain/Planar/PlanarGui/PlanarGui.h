#pragma once
#include "./../../../../Common/Vendor/imgui/imgui.h"
#include "./../Planar.h"
inline void planarTerrainGuiRender(PlanarTerrainRenderProperties &properties) {
	ImGui::Begin("Right Content", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	if (ImGui::TreeNode("Globe Properties")) {
		ImGui::ColorEdit3("Light Color", glm::value_ptr(properties.lightColor));
		ImGui::DragFloat3("Light Position", glm::value_ptr(properties.lightPos), 1000.0f);
		ImGui::ColorEdit3("Globe Color", glm::value_ptr(properties.objectColor));
		ImGui::TreePop();
	}
	ImGui::End();
}