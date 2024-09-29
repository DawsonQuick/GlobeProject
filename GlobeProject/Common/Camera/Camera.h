#pragma once
#include "./CameraDataStructs/CameraDataStructs.h"
#include "./CameraTypes/OrbitalCam/OrbitalCam.h"
#include "./CameraTypes/FreeCam/FreeCam.h"
#pragma optimize("", off)
class Camera
{
public:
	Camera(GLFWwindow* window, CameraType type);
	~Camera();


	void update(GLFWwindow* window, float currentFrame);
	CameraInfo& getCameraInfo();
	float getCameraRadius();
	void setCameraRadius(float value);
private:
	std::unique_ptr<ICamera> camImpl;
	std::unique_ptr<ICameraControls> controls;

};
#pragma optimize("", on)