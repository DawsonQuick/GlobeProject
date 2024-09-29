#pragma once
// GLFW
#include <GLFW/glfw3.h>
#include "./../CameraDataStructs/CameraDataStructs.h"
#include "./../../ChunkManager/ChunkManager.h"

class ICamera {
public:
	virtual ~ICamera() = default;
	virtual void update(GLFWwindow* window, float currentFrame) = 0;
	virtual float getCameraRadius(){ return 0.0f; };
	virtual void setCameraRadius(float value) = 0;

	CameraInfo& getCameraInfo() {
		return m_CameraInfo;
	}

	void serialize() {

	}

protected:
	CameraInfo m_CameraInfo;
};