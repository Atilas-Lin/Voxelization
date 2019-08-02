#include "Camera.h"

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Forward, Up);
}

// Processes input received from any keyboard-like input system
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	if (direction == FORWARD)
		Position += Forward * velocity;
	if (direction == BACKWARD)
		Position -= Forward * velocity;
	if (direction == LEFT)
		Position -= Right * velocity;
	if (direction == RIGHT)
		Position += Right * velocity;
	if (direction == UP)
		Position += Up * velocity;
	if (direction == DOWN)
		Position -= Up * velocity;
	
	//UpdateCameraPos();
}

// Processes input received from a mouse input system.
// Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}
	// Update Forward, Right and Up Vectors using the update Euler angles
	UpdateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event.
// Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset)
{
	if (Zoom >= 1.0f && Zoom <= 45.0f)
		Zoom -= yoffset;
	if (Zoom <= 1.0f)
		Zoom = 1.0f;
	if (Zoom >= 45.0f)
		Zoom = 45.0f;
}

void Camera::UpdateCameraPos()
{
	Position += Forward * speedZ * 0.1f + Right * speedX * 0.1f + Up * speedY * 0.1f;
}

// Caculates the forward vector from the Camera's (update) Euler Angles
void Camera::UpdateCameraVectors()
{
	// Caculate the new Forward vector
	glm::vec3 forward;
	forward.x = glm::cos(glm::radians(Pitch))*glm::sin(glm::radians(Yaw)); //¥Îradians´«¦¨©·«×
	forward.z = glm::cos(glm::radians(Pitch))*glm::cos(glm::radians(Yaw));
	forward.y = glm::sin(glm::radians(Pitch));
	Forward = glm::normalize(forward);
	// Also re-caculate the Right and Up vector
	Right = glm::normalize(glm::cross(Forward, WorldUp)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Forward)); // *A cross B != B cross A
}
