#include "Camera.h"

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

glm::mat4 Camera::Calculate_LookAt_Matrix(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
{
	return glm::mat4();
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = m_MovementSpeed * deltaTime;
	if (direction == Camera_Movement::FORWARDS)
		m_Position += m_Front * velocity;
	if (direction == Camera_Movement::BACKWARDS)
		m_Position -= m_Front * velocity;
	if (direction == Camera_Movement::LEFT)
		m_Position -= m_Right * velocity;
	if (direction == Camera_Movement::RIGHT)
		m_Position += m_Right * velocity;
	if (direction == Camera_Movement::UP)
		m_Position += m_Up * velocity;
	if (direction == Camera_Movement::DOWN)
		m_Position -= m_Up * velocity;

	// To make sure the user stays at the ground level
	// m_Position.y = 0.0f; // <-- this one-liner keeps the user at the ground level (xz plane)
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrinPitch)
{
	xoffset *= m_MouseSensibility;
	yoffset *= m_MouseSensibility;

	m_Yaw   += xoffset;
	m_Pitch += yoffset;

	// Make sure that when pitch is out of bound, screen doesn't get flipped
	if (constrinPitch)
	{
		if (m_Pitch > +89.0f)
			m_Pitch = +89.0f;
		if (m_Pitch < -89.0f)
			m_Pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

void Camera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(m_Pitch)) * cos(glm::radians(m_Yaw));
	front.y = sin(glm::radians(m_Pitch));
	front.z = cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw));
	m_Front = glm::normalize(front);
	// Also re-calculate the Right and Up Vector
	// Normalize the vectors, because their lenght gets closer to 0 the more you look up or down which results in slower movement.
	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up    = glm::normalize(glm::cross(m_Right, m_Front));
}
