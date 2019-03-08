#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Default camera values
const float YAW         = -90.0f;
const float PITCH       = +0.0f;
const float SPEED       = +2.5f;
const float SENSIBILITY = +0.1f;
const float ZOOM        = +45.0f;

class Camera
{
public:
	// Defines severaal possible options for camera movement.
	// Used as abstraction to stay away from window-system specific input methods
	enum Camera_Movement
	{
		FORWARDS = 0, BACKWARDS, LEFT, RIGHT, UP, DOWN,
	};

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
		: m_Position(position), m_WorldUp(up), m_Yaw(yaw), m_Pitch(pitch),
		m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), m_MovementSpeed(SPEED), m_MouseSensibility(SENSIBILITY), m_Zoom(ZOOM) {}


	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix();
	glm::mat4 Calculate_LookAt_Matrix(glm::vec3 Position, glm::vec3 target, glm::vec3 worldUp);

	// Proccesses input received from any keyboard-like input system. Accepts input parameter in the form of 
	// camera defined ENUM ( to abstract it from windowing system)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);

	// Process input received from a mouse input system. Except the offset value in both the x and y direction
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrinPitch = true);

	inline void ResetYawPitch() { m_Yaw = YAW, m_Pitch = PITCH; }

private:
	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_WorldUp;
	// Euler Angles
	float m_Yaw, m_Pitch;
	// Camera Options
	float m_MovementSpeed, m_MouseSensibility, m_Zoom;
private:
	// Calculate the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors();

};