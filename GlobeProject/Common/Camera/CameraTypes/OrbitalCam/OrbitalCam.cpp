#pragma optimize("", off)
#include "OrbitalCam.h"

OrbitalCam::OrbitalCam(GLFWwindow* window):
m_Yaw(-90.0f), m_Pitch(0), m_FirstMouse(true),
m_LastX(1920.0f / 2.0f), m_LastY(1080.0f / 2.0f),
m_Radius(20000.0f), m_CentralPoint(0.0f, 0.0f, 0.0f),
m_Pressed(false), window(window)
{
    m_CameraInfo.type = CameraType::ORBITAL; //Set the ICamera
}
OrbitalCam::~OrbitalCam() {

}

float OrbitalCam::getCameraRadius() {
    return m_Radius;
}

void OrbitalCam::setCameraRadius(float value) {
    m_Radius = value;
    updateCameraVectors();
}

void OrbitalCam::update(GLFWwindow* window, float deltaTime) {
    glfwGetWindowSize(this->window, &m_WindowWidth, &m_WindowHeight);
    double tempX;
    double tempY;
    glfwGetCursorPos(this->window, &tempX, &tempY);
    m_LastX = tempX;
    m_LastY = tempY;

    ChunkManager::updateCameraPosition(m_CameraInfo.m_CameraPos);
}
void OrbitalCam::mouse_Dragcallback()
{ 
    glfwGetCursorPos(this->window, &m_MouseX, &m_MouseY);
    float xpos = static_cast<float>(m_MouseX);
    float ypos = static_cast<float>(m_MouseY);

    float xoffset = xpos - m_LastX;
    float yoffset = m_LastY - ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    m_Yaw += xoffset;
    m_Pitch += yoffset;

    if (m_Pitch > 89.0f) m_Pitch = 89.0f;
    if (m_Pitch < -89.0f) m_Pitch = -89.0f;

    updateCameraVectors();
}

void OrbitalCam::updateCameraVectors() {
    // Calculate the new camera position
    glm::vec3 position;
    position.x = m_CentralPoint.x + m_Radius * cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    position.y = m_CentralPoint.y + m_Radius * sin(glm::radians(m_Pitch));
    position.z = m_CentralPoint.z + m_Radius * sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    m_CameraInfo.m_CameraPos = position;
    // Camera front vector always looks at the central point
    m_CameraInfo.m_CameraFront = glm::normalize(m_CentralPoint - position);

    m_CameraInfo.view = glm::lookAt(position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_CameraInfo.projection = glm::perspective(glm::radians(45.0f), (float)m_WindowWidth / (float)m_WindowHeight, 1000.0f, 10000000.0f);
}

void OrbitalCam::calculateRay() {

    glfwGetCursorPos(this->window, &m_MouseX, &m_MouseY);
    // Convert screen coordinates to normalized device coordinates
    float x = (2.0f * m_MouseX) / m_WindowWidth - 1.0f;
    float y = 1.0f - (2.0f * m_MouseY) / m_WindowHeight;
    float z = 1.0f;
    glm::vec3 ray_nds = glm::vec3(x, y, z);

    // Convert to clip coordinates
    glm::vec4 ray_clip = glm::vec4(ray_nds, 1.0);

    // Convert to eye coordinates
    glm::vec4 ray_eye = glm::inverse(m_CameraInfo.projection) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

    // Convert to world coordinates
    glm::vec3 ray_wor = glm::vec3(glm::inverse(m_CameraInfo.view) * ray_eye);
    ray_wor = glm::normalize(ray_wor);

    Ray ray;
    ray.origin = m_CameraInfo.m_CameraPos;
    ray.direction = ray_wor;

    std::vector<float> rayLine;
    rayLine.push_back(ray.origin.x);
    rayLine.push_back(ray.origin.y);
    rayLine.push_back(ray.origin.z);

    float deltaX = 10000000.0f * ray.direction.x;
    float deltaY = 10000000.0f * ray.direction.y;
    float deltaZ = 10000000.0f * ray.direction.z;

    float finalX = ray.origin.x + deltaX;
    float finalY = ray.origin.y + deltaY;
    float finalZ = ray.origin.z + deltaZ;

    rayLine.push_back(finalX);
    rayLine.push_back(finalY);
    rayLine.push_back(finalZ);

    ray.lineVertices = rayLine;
    ray.end = glm::vec3(finalX, finalY, finalZ);

    ChunkManager::findRayIntersectEntity(ray);
    m_CameraInfo.isRaySet = true;
    m_CameraInfo.ray = ray;
}
#pragma optimize("", on)