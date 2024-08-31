#pragma once
#ifndef UNIVERSALSTRUCTS_H
#define UNIVERSALSTRUCTS_H
#include "./../../Common/Vendor/glm/glm.hpp"

struct RenderTransferData {
	glm::mat4 transform;
	glm::vec3 color;
	//Default Contructor
	RenderTransferData() {}
	RenderTransferData(glm::mat4 transform, glm::vec3 color) : transform(transform), color(color) {

	}
};


struct RenderTransferDataTemp {
	std::vector<RenderTransferData> instanceInformation;
	int modelId;


	std::vector<RenderTransferData>& getInstanceInformation(){
		return instanceInformation;
	}
	//Default Contructor
	RenderTransferDataTemp() {}

	RenderTransferDataTemp(std::vector<RenderTransferData> instanceInformation, int modelId) : modelId(modelId), instanceInformation(instanceInformation) {

	}
};



#endif