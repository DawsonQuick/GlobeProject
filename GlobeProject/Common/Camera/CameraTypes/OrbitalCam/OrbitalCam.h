#pragma once
#include "./../CameraTypeInterface.h"
#pragma optimize("", off)
class OrbitalCam : public ICamera {
public:
	OrbitalCam(GLFWwindow* window);
	~OrbitalCam();

	void update(GLFWwindow* window, float deltaTime) override;
	float getCameraRadius() override;
	void setCameraRadius(float value) override;

private:
	bool m_FirstMouse, m_trackingEnabled;
	float m_Yaw, m_Pitch, m_LastX, m_LastY;
	float m_Radius;
	double m_MouseX, m_MouseY;
	GLFWwindow* window;

	bool m_Dragging, m_Pressed;
	glm::vec3 m_CentralPoint;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_PressStartTime;
	//GLFW window size
	int m_WindowWidth;
	int m_WindowHeight;

	void calculateRay();
	void updateCameraVectors();
	void mouse_Dragcallback();

	friend struct OribitalControls;
};


struct OribitalControls : public ICameraControls {


	/*
	* TODO: Add keybinds for operating camera
	*
	* Default Right Mouse (Hold): used to rotate the camera around the origin of the radius
	* Default Left Mouse (Press): use to cast a ray out from the camera position into the world (world interaction)
	*
	*/



	/*
	* Variables:
	*
	* float controls_CameraRadius
	*
	*/

	OrbitalCam* camera;  // Pointer to the camera instance

	OribitalControls(OrbitalCam* cam) : camera(cam) {
		camera->m_Radius = 7000;
	}

	void bindKeys() override {
		// Bind the mouse button to trigger calculateRay
		IO::addKeyToCheckFor("Primary Click",
			{ GLFW_MOUSE_BUTTON_1 },
			[this]() { camera->calculateRay(); },  // Capture 'this' and call camera->calculateRay
			IO::OnPress, IO::MouseButtonInput, false
		);

		IO::addKeyToCheckFor("Drag Camera",
			{ GLFW_MOUSE_BUTTON_2 },
			[this]() { camera->mouse_Dragcallback(); },
			IO::OnHold, IO::MouseButtonInput, false
		);
	}

	void unbindKeys() override {

	}

	void renderGuiControls() override {

	}

};

#pragma optimize("", on)