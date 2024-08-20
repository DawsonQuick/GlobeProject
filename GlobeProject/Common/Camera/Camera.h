#pragma once
// GLFW
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include "../Vendor/glm/glm.hpp"
#include "../Vendor/glm/gtc/matrix_transform.hpp"
#include "../Vendor/glm/gtc/type_ptr.hpp"
#include "./../../Common/OpenGLUtilities/LineRenderer/LineRenderer.h"
#include "./../WorldInteraction/WorldInteraction.h"
struct CameraInfo {
	LineRenderer lineRenderer;

	glm::vec3 m_CameraPos = glm::vec3(100, 0, 100);
	glm::vec3 m_CameraFront = glm::vec3(0, 0, -1);
	glm::vec3 m_CameraUp = glm::vec3(0, 1, 0);

	glm::mat4 view;
	glm::mat4 projection;

	bool isRaySet;
	Ray ray;
	void renderRay() {
		if (isRaySet) {
			lineRenderer.appendLines(ray.lineVertices);
			lineRenderer.render(glm::mat4(1.0f), view, projection);
		}
	}

	CameraInfo() : lineRenderer(false){

	}

};

class Camera
{
public:
	Camera(int id);
	~Camera();
	void update(GLFWwindow* window, float currentFrame);
	CameraInfo getCameraInfo();
	void processInput(GLFWwindow* window);
	void mouse_callback(double xposIn, double yposIn);
	void mouse_Dragcallback(double xposIn, double yposIn);
	void updateCameraVectors();
	float getCameraRadius();
	void setCameraRadius(float value);
	void calculateRay(double mouseX, double mouseY, int screenWidth, int screenHeight);
private:
	CameraInfo m_CameraInfo;
	bool m_FirstMouse, m_trackingEnabled;
	float m_DeltaTime, m_LastFrame, m_Yaw, m_Pitch, m_LastX, m_LastY;
	float m_Radius;
	bool m_Dragging, m_Pressed;
	glm::vec3 m_CentralPoint;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_PressStartTime;
	//GLFW window size
	int m_WindowWidth;
	int m_WindowHeight;

};

