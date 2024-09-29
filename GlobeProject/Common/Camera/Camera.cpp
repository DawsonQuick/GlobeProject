#pragma optimize("", off)
#include "Camera.h"
#include "./../Vendor/imgui/imgui.h"

Camera::Camera(GLFWwindow* window,CameraType type)
{
    switch (type) {
    case CameraType::ORBITAL: {
        // Create an instance of OrbitalCam and pass it to OribitalControls
        auto orbitalCam = std::make_unique<OrbitalCam>(window);
        auto orbitalControls = std::make_unique<OribitalControls>(orbitalCam.get());  // Pass the raw pointer to controls

        // Bind the keys for the Orbital Controls
        orbitalControls->bindKeys();

        // Store the unique pointers in camImpl and controls
        camImpl = std::move(orbitalCam);
        controls = std::move(orbitalControls);

        break;
    }
    case CameraType::FREECAM: {
        // Create an instance of OrbitalCam and pass it to OribitalControls
        auto freeCam = std::make_unique<FreeCam>(window);
        auto freeCamControls = std::make_unique<FreeCamControls>(freeCam.get());  // Pass the raw pointer to controls

        // Bind the keys for the Orbital Controls
        freeCamControls->bindKeys();

        // Store the unique pointers in camImpl and controls
        camImpl = std::move(freeCam);
        controls = std::move(freeCamControls);

        break;
    }

    }
}
Camera::~Camera() {
    camImpl->~ICamera();
}
void Camera::update(GLFWwindow* window, float deltaTime) {
    camImpl->update(window, deltaTime);
}
CameraInfo& Camera::getCameraInfo() {
    return camImpl->getCameraInfo();
}
float Camera::getCameraRadius() {
    return camImpl->getCameraRadius();
}
void Camera::setCameraRadius(float value) {
    camImpl->setCameraRadius(value);
}
#pragma optimize("", on)