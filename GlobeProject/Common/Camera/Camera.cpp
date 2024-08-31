#pragma optimize("", off)  // Disable optimizations for the Camera class
#include "Camera.h"
#include "./../Vendor/imgui/imgui.h"


Camera::Camera(int id) :
    m_DeltaTime(0.0f), m_LastFrame(0.0f),
    m_Yaw(-90.0f), m_Pitch(0), m_FirstMouse(true),
    m_LastX(1920.0f / 2.0f), m_LastY(1080.0f / 2.0f),
    m_Radius(20000.0f), m_CentralPoint(0.0f, 0.0f, 0.0f),
    m_Pressed(false)

{
}
Camera::~Camera() {

}

void Camera::update(GLFWwindow* window, float currentFrame) {
    glfwGetWindowSize(window, &m_WindowWidth, &m_WindowHeight);
    m_DeltaTime = currentFrame - m_LastFrame;
    m_LastFrame = currentFrame;
    //Call updates on all the hardware inputs
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    //mouse_callback(mouseX, mouseY);
    processInput(window);

    if (ImGui::IsMouseHoveringAnyWindow() || ImGui::IsAnyItemActive()) {
        return;
    }
    if (m_Dragging || m_FirstMouse) {
        mouse_Dragcallback(mouseX, mouseY);
    }

    ChunkManager::updateCameraPosition(m_CameraInfo.m_CameraPos);
    
}

CameraInfo Camera::getCameraInfo() {
    return m_CameraInfo;
}

//Presses keyboard inputs
void Camera::processInput(GLFWwindow* window)
{
    static bool wasTabPressed = false; // Track previous state of Tab key
    static auto lastToggleTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastToggleTime);

    //Disable this callback if interacting with a ImGUI element
    if (!(ImGui::IsMouseHoveringAnyWindow() || ImGui::IsAnyItemActive())) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (!m_Pressed) {
                m_Pressed = true;
                m_PressStartTime = std::chrono::high_resolution_clock::now();
            }
            if (m_Pressed) {
                if ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_PressStartTime)) > std::chrono::milliseconds(15)) {
                    m_Dragging = true;
                }
            }
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            m_Dragging = false;
            m_FirstMouse = true; // Reset first mouse use
            if (m_Pressed) {
                m_Pressed = false;
                if ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_PressStartTime)) < std::chrono::milliseconds(150)) {
                    calculateRay(m_LastX, m_LastY, m_WindowWidth, m_WindowHeight);
                }
            }
        }
    }
    // Check Tab key state
    int tabState = glfwGetKey(window, GLFW_KEY_TAB);

    // Detect transition from pressed to released
    if (tabState == GLFW_PRESS && !wasTabPressed) {
    }
    else if (tabState == GLFW_RELEASE && wasTabPressed) {
        // The key was released this frame and was pressed last frame
        m_CameraInfo.isLogToggled = !m_CameraInfo.isLogToggled;
    }

    // Update the previous state of the Tab key
    wasTabPressed = (tabState == GLFW_PRESS);

}

//Processes mouse events and location
void Camera::mouse_callback(double xposIn, double yposIn)
{
    //If the mouse tracking toggle is enables
    if (m_trackingEnabled) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (m_FirstMouse)
        {
            m_LastX = xpos;
            m_LastY = ypos;
            m_FirstMouse = false;
        }

        float xoffset = xpos - m_LastX;
        float yoffset = m_LastY - ypos; // reversed since y-coordinates go from bottom to top
        m_LastX = xpos;
        m_LastY = ypos;

        float sensitivity = 0.1f; // change this value to your liking
        xoffset *= sensitivity;
        yoffset *= sensitivity;


        m_Yaw += xoffset;
        m_Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        m_CameraInfo.m_CameraFront = glm::normalize(front);
    }
}

void Camera::mouse_Dragcallback(double xposIn, double yposIn)
{
    if (m_trackingEnabled) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (m_FirstMouse) {
            m_LastX = xpos;
            m_LastY = ypos;
            m_FirstMouse = false;
        }

        float xoffset = xpos - m_LastX;
        float yoffset = m_LastY - ypos;
        m_LastX = xpos;
        m_LastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        m_Yaw += xoffset;
        m_Pitch += yoffset;

        if (m_Pitch > 89.0f) m_Pitch = 89.0f;
        if (m_Pitch < -89.0f) m_Pitch = -89.0f;

        updateCameraVectors();
    }
}

void Camera::updateCameraVectors() {
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

float Camera::getCameraRadius() {
    return m_Radius;
}

void Camera::setCameraRadius(float value) {
    m_Radius = value;
    updateCameraVectors();
}
void Camera::calculateRay(double mouseX, double mouseY, int screenWidth, int screenHeight) {
    // Convert screen coordinates to normalized device coordinates
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight;
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
