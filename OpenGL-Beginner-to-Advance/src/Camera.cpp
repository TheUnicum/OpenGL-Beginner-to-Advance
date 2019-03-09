#include "Camera.h"

glm::mat4 Camera::Calculate_LookAt_Matrix(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
{
	/*
	         +-          -+  +-           -+
	         | Rx Ry Rz 0 |  | 1  0  0 -Px |
	LookAt = | Ux Uy Uz 0 |  | 0  1  0 -Py |
	         | Dx Dy Dz 0 |  | 0  0  1 -Pz |
	         | 0  0  0  1 |  | 0  0  0  1  |
	         +-          -+  +-           -+
	*/
	glm::vec3 zaxis_D = glm::normalize(position - target);
	glm::vec3 xaxis_R = glm::normalize(glm::cross(worldUp, zaxis_D));
	glm::vec3 yaxis_U = glm::normalize(glm::cross(zaxis_D, xaxis_R));

	// Create translation and rotation matrix
	// In glm we access elements as mat[col][row] due to clumn-major layout
	glm::mat4 translation(1.0f);
	translation[3][0] = -position.x; // Third colum, first row
	translation[3][1] = -position.y;
	translation[3][2] = -position.z;
	// translation[3][3] = 1.0f; non serve perche nella matrix identity
	glm::mat4 rotation(1.0f);
	rotation[0][0] = xaxis_R.x;
	rotation[1][0] = xaxis_R.y;
	rotation[2][0] = xaxis_R.z;

	rotation[0][1] = yaxis_U.x;
	rotation[1][1] = yaxis_U.y;
	rotation[2][1] = yaxis_U.z;

	rotation[0][2] = zaxis_D.x;
	rotation[1][2] = zaxis_D.y;
	rotation[2][2] = zaxis_D.z;
	//rotation[3][3] = 1.0f;

	return rotation * translation;;
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
	m_Position.y = (m_Fix_To_Ground ? 0 : 1) * m_Position.y;
}

void Camera::ProcessKeyboard(glm::vec3 direction, float deltaTime)
{
	float velocity = m_MovementSpeed * deltaTime;
	m_Position += (-m_Front * direction.z + m_Right * direction.x + m_Up * direction.y) * velocity ;
	m_Position.y = (m_Fix_To_Ground ? 0 : 1) * m_Position.y;
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
