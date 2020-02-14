#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// Constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}


glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == Camera_Movement::FORWARD) { Position += Front * velocity; }
    if (direction == Camera_Movement::BACKWARD) { Position -= Front * velocity; }
    if (direction == Camera_Movement::LEFT) { Position -= Right * velocity; }
    if (direction == Camera_Movement::RIGHT) { Position += Right * velocity; }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;
    Yaw += xoffset;
    Pitch += yoffset;

    // make sure when the pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.f) { Pitch = 89.f; }
        if (Pitch < -89.f) { Pitch = -89.f; }
    }
    
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    if (Zoom >= 1.f && Zoom <= 45.f) { Zoom -= yoffset; }
    if (Zoom <= 1.f) { Zoom = 1.f; }
    if (Zoom >= 45.f) { Zoom = 45.f; }
}

void Camera::updateCameraVectors()
{
    // calculate the new front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also calculate the right and up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
