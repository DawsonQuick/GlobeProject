#pragma once
#include "./../CameraTypeInterface.h"

class FreeCam : public ICamera {
public:
	FreeCam(GLFWwindow* window);
	~FreeCam();

	void update(GLFWwindow* window, float deltaTime) override;
	float getCameraRadius() override;
	void setCameraRadius(float value) override;
private:

	bool m_FirstMouse, m_trackingEnabled;
	float m_LastX, m_LastY;
	float m_Radius;
	double m_MouseX, m_MouseY;
	GLFWwindow* window;
	float m_SmoothedYaw = 0.0f;
	float m_SmoothedPitch = 0.0f;
	float smoothingFactor = 0.1f;
	bool m_Dragging, m_Pressed;
	glm::vec3 m_CentralPoint;

	//GLFW window size
	int m_WindowWidth;
	int m_WindowHeight;

	float m_AccumulatedX;
	float m_AccumulatedY;

	float m_deltaTime;

	float cameraSpeed;
	bool mouseControlEnabled;

	void updateCursorPosition();
	void calculateRay();
	void updateCameraVectors();
	void mouse_Dragcallback();
	void camera_MoveFoward();
	void camera_MoveBackwards();
	void camera_MoveLeft();
	void camera_MoveRight();
	void camera_MoveUp();
	void camera_MoveDown();

	void toggleMouseControl();

	friend struct FreeCamControls;
};

struct FreeCamControls : public ICameraControls {

	/*
	* Keybinds for operating camera
	*
	* Default Left Mouse (Press): use to cast a ray out from the camera position into the world (world interaction)
	* Movement: (based on current camera rotation and angle)
	*	Default W: Move foward
	*	Default A: Move left
	*	Default S: Move back
	*	Default D: Move right
	*
	* (Note) With this configuration the mouse delta per frame should always be captured
	*/

	FreeCam* camera;

	FreeCamControls(FreeCam* camera): camera(camera){

	}

	void bindKeys() override {
		// Bind the mouse button to trigger calculateRay
		IO::addKeyToCheckFor("Primary Click",
			{ GLFW_MOUSE_BUTTON_1 },
			[this]() { camera->calculateRay(); },  // Capture 'this' and call camera->calculateRay
			IO::OnPress, IO::MouseButtonInput, true
		);

		IO::addKeyToCheckFor("Camera Move Foward",
			{ GLFW_KEY_W },
			[this]() { camera->camera_MoveFoward(); },
			IO::OnHold, IO::KeyboardInput, true
		);

		IO::addKeyToCheckFor("Camera Move Backwards",
			{ GLFW_KEY_S },
			[this]() { camera->camera_MoveBackwards(); },
			IO::OnHold, IO::KeyboardInput, true
		);

		IO::addKeyToCheckFor("Camera Move Left",
			{ GLFW_KEY_A },
			[this]() { camera->camera_MoveLeft(); },
			IO::OnHold, IO::KeyboardInput, true
		);

		IO::addKeyToCheckFor("Camera Move Right",
			{ GLFW_KEY_D },
			[this]() { camera->camera_MoveRight(); },
			IO::OnHold, IO::KeyboardInput, true
		);

		IO::addKeyToCheckFor("Camera Move Up",
			{ GLFW_KEY_SPACE },
			[this]() { camera->camera_MoveUp(); },
			IO::OnHold, IO::KeyboardInput, true
		);

		IO::addKeyToCheckFor("Camera Move Down",
			{ GLFW_KEY_LEFT_SHIFT },
			[this]() { camera->camera_MoveDown(); },
			IO::OnHold, IO::KeyboardInput, true
		);

		IO::addKeyToCheckFor("Toggle Free Camera",
			{ GLFW_KEY_ESCAPE },
			[this]() { camera->toggleMouseControl(); },
			IO::OnPress, IO::KeyboardInput, true
		);
	}

	void unbindKeys() override {

	}

	void renderGuiControls() override {

	}

};