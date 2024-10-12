#include "FreeCam.h"

FreeCam::FreeCam(GLFWwindow* window):
    m_FirstMouse(true),
    m_LastX(1920.0f / 2.0f), m_LastY(1080.0f / 2.0f),
    m_Radius(20000.0f), m_CentralPoint(0.0f, 0.0f, 0.0f),
    m_Pressed(false), window(window), cameraSpeed(50), mouseControlEnabled(false)
{

    m_CameraInfo.type = CameraType::FREECAM;

    if (!mouseControlEnabled) {
        // Hide the mouse and lock it to the window
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else if (mouseControlEnabled) {
        // Make the mouse visible and allow it to move freely again
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

}

FreeCam::~FreeCam() {

}

float FreeCam::getCameraRadius() {
    return m_Radius;
}

void FreeCam::setCameraRadius(float value) {
    m_Radius = value;
    updateCameraVectors();
}

void FreeCam::update(GLFWwindow* window, float deltaTime) {
    glfwGetWindowSize(this->window, &m_WindowWidth, &m_WindowHeight);
    if (!mouseControlEnabled) {
        mouse_Dragcallback();
    }

    ChunkManager::updateCameraPosition(m_CameraInfo.m_CameraPos);
}


void FreeCam::updateCursorPosition() {
    if (mouseControlEnabled) {
        glfwSetCursorPos(window, m_WindowWidth / 2.0, m_WindowHeight / 2.0);
    }
}

void FreeCam::mouse_Dragcallback() {

    updateCursorPosition(); // Keep the mouse at the center of the screen, even though it should be hidden

    glfwGetCursorPos(this->window, &m_MouseX, &m_MouseY);
    float xpos = static_cast<float>(m_MouseX);
    float ypos = static_cast<float>(m_MouseY);

    float xoffset = xpos - m_LastX;
    float yoffset = m_LastY - ypos;

    // Update last mouse position
    m_LastX = xpos;
    m_LastY = ypos;

    // Adjust sensitivity
    float sensitivity = 0.5f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Update yaw and pitch with offsets
    float newYaw = m_CameraInfo.m_Yaw + xoffset;
    float newPitch = m_CameraInfo.m_Pitch + yoffset;

    // Constrain pitch to avoid flipping
    if (newPitch > 89.0f) newPitch = 89.0f;
    if (newPitch < -89.0f) newPitch = -89.0f;

    // Smooth the yaw and pitch using interpolation (exponential smoothing)
    m_SmoothedYaw = (1.0f - smoothingFactor) * m_SmoothedYaw + smoothingFactor * newYaw;
    m_SmoothedPitch = (1.0f - smoothingFactor) * m_SmoothedPitch + smoothingFactor * newPitch;

    // Update the actual camera yaw and pitch with the smoothed values
    m_CameraInfo.m_Yaw = m_SmoothedYaw;
    m_CameraInfo.m_Pitch = m_SmoothedPitch;

    // Update the camera's front direction based on the new yaw and pitch
    updateCameraVectors();
}


void FreeCam::updateCameraVectors() {
    // Calculate the new front vector
    glm::vec3 front;
    front.x = cos(glm::radians(m_CameraInfo.m_Yaw)) * cos(glm::radians(m_CameraInfo.m_Pitch));
    front.y = sin(glm::radians(m_CameraInfo.m_Pitch));
    front.z = sin(glm::radians(m_CameraInfo.m_Yaw)) * cos(glm::radians(m_CameraInfo.m_Pitch));
    m_CameraInfo.m_CameraFront = glm::normalize(front);  // Normalize the front vector

    // Calculate right and up vectors based on the front vector
    m_CameraInfo.m_CameraRight = glm::normalize(glm::cross(m_CameraInfo.m_CameraFront, glm::vec3(0.0f, 1.0f, 0.0f))); // Right vector
    m_CameraInfo.m_CameraUp = glm::normalize(glm::cross(m_CameraInfo.m_CameraRight, m_CameraInfo.m_CameraFront)); // Up vector

    // View matrix
    m_CameraInfo.view = glm::lookAt(m_CameraInfo.m_CameraPos, m_CameraInfo.m_CameraPos + m_CameraInfo.m_CameraFront, m_CameraInfo.m_CameraUp);

    // Perspective projection matrix
    m_CameraInfo.projection = glm::perspective(glm::radians(45.0f), (float)m_WindowWidth / (float)m_WindowHeight, 10.0f, 10000000.0f);
}

void FreeCam::calculateRay() {
    if (!mouseControlEnabled) {
        // Ray originates from the camera position
        glm::vec3 rayOrigin = m_CameraInfo.m_CameraPos;

        // Ray direction is the direction the camera is looking (normalized front vector)
        glm::vec3 rayDirection = glm::normalize(m_CameraInfo.m_CameraFront);

        // Initialize Ray structure
        Ray ray;
        ray.origin = rayOrigin;
        ray.direction = rayDirection;

        // Prepare to visualize the ray as a line (optional)
        std::vector<float> rayLine;
        rayLine.push_back(ray.origin.x);
        rayLine.push_back(ray.origin.y);
        rayLine.push_back(ray.origin.z);

        // Extend the ray by a large distance for visualization or intersection testing
        float deltaX = 10000000.0f * ray.direction.x;
        float deltaY = 10000000.0f * ray.direction.y;
        float deltaZ = 10000000.0f * ray.direction.z;

        float finalX = ray.origin.x + deltaX;
        float finalY = ray.origin.y + deltaY;
        float finalZ = ray.origin.z + deltaZ;

        rayLine.push_back(finalX);
        rayLine.push_back(finalY);
        rayLine.push_back(finalZ);

        // Store the final ray line and endpoint for further use
        ray.lineVertices = rayLine;
        ray.end = glm::vec3(finalX, finalY, finalZ);

        // Call your chunk or entity intersection function (or any further logic)
        ChunkManager::findRayIntersectEntity(ray);

        // Set the ray for rendering or debugging purposes
        m_CameraInfo.isRaySet = true;
        m_CameraInfo.ray = ray;
    }
}

void FreeCam::camera_MoveFoward() {
    if (!mouseControlEnabled) {
        m_CameraInfo.m_CameraPos += cameraSpeed * m_CameraInfo.m_CameraFront;
    }
}
void FreeCam::camera_MoveBackwards() {
    if (!mouseControlEnabled) {
        m_CameraInfo.m_CameraPos -= cameraSpeed * m_CameraInfo.m_CameraFront;
    }
}
void FreeCam::camera_MoveLeft() {
    if (!mouseControlEnabled) {
        m_CameraInfo.m_CameraPos -= cameraSpeed * m_CameraInfo.m_CameraRight;
    }
}
void FreeCam::camera_MoveRight() {
    if (!mouseControlEnabled) {
        m_CameraInfo.m_CameraPos += cameraSpeed * m_CameraInfo.m_CameraRight;
    }
}
void FreeCam::camera_MoveUp() {
    if (!mouseControlEnabled) {
        m_CameraInfo.m_CameraPos += cameraSpeed * m_CameraInfo.m_CameraUp;
    }
}
void FreeCam::camera_MoveDown() {
    if (!mouseControlEnabled) {
        m_CameraInfo.m_CameraPos -= cameraSpeed * m_CameraInfo.m_CameraUp;
    }
}

void FreeCam::toggleMouseControl() {
    mouseControlEnabled = !mouseControlEnabled;

    if (!mouseControlEnabled) {

        //Set the cursor location to the last location is was set to before disabling tracking
        glfwSetCursorPos(window, m_LastX, m_LastY);

        // Hide the mouse and lock it to the window
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else {
        // Make the mouse visible and allow it to move freely again
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        // Set the last known mouse position to the center of the screen
        m_LastX = m_WindowWidth / 2.0f;
        m_LastY = m_WindowHeight / 2.0f;

        // Set the actual cursor position to the center of the screen
        glfwSetCursorPos(window, m_LastX, m_LastY);

    }
}

