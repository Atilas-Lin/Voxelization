#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default camera values
const float YAW = 180.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Forward;
	glm::vec3 Right;
	glm::vec3 Up;
	glm::vec3 WorldUp;
	// Euler angles
	float Pitch;
	float Yaw;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	float speedX;
	float speedY;
	float speedZ;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 up = glm::vec3(0, 1.0f, 0), float yaw = YAW, float pitch = PITCH) :Forward(glm::vec3(0, 0, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Pitch = pitch;
		Yaw = yaw;
		UpdateCameraVectors();
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :Forward(glm::vec3(0, 0, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		UpdateCameraVectors();
	}
	~Camera() {
	}

	glm::mat4 GetViewMatrix();
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
	void ProcessMouseScroll(float yoffset);
	void UpdateCameraPos();

private:
	void UpdateCameraVectors();
};

