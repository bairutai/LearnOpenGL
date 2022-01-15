#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include "export.h"
// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class CameraImpl
{
public:
	// camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
};

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class DLL_API Camera
{
public:
	// euler Angles
	float Yaw;
	float Pitch;
	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
		: MovementSpeed(SPEED)
		, MouseSensitivity(SENSITIVITY)
		, Zoom(ZOOM)
	{
		m_pCameraImpl = new CameraImpl();
		m_pCameraImpl->Position = position;
		m_pCameraImpl->WorldUp = up;
		m_pCameraImpl->Front = glm::vec3(0.0f, 0.0f, -1.0f);

		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
		: MovementSpeed(SPEED)
		, MouseSensitivity(SENSITIVITY)
		, Zoom(ZOOM)
	{
		m_pCameraImpl = new CameraImpl();
		m_pCameraImpl->Position = glm::vec3(posX, posY, posZ);
		m_pCameraImpl->WorldUp = glm::vec3(upX, upY, upZ);
		m_pCameraImpl->Front = glm::vec3(0.0f, 0.0f, -1.0f);

		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	~Camera()
	{
		delete m_pCameraImpl;
		m_pCameraImpl = nullptr;
	}

	glm::vec3 getPosition()
	{
		return m_pCameraImpl->Position;
	}

	glm::vec3 getFront()
	{
		return m_pCameraImpl->Front;
	}

	// returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(m_pCameraImpl->Position, m_pCameraImpl->Position + m_pCameraImpl->Front, m_pCameraImpl->Up);
	}

	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			m_pCameraImpl->Position += m_pCameraImpl->Front * velocity;
		if (direction == BACKWARD)
			m_pCameraImpl->Position -= m_pCameraImpl->Front * velocity;
		if (direction == LEFT)
			m_pCameraImpl->Position -= m_pCameraImpl->Right * velocity;
		if (direction == RIGHT)
			m_pCameraImpl->Position += m_pCameraImpl->Right * velocity;
	}

	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}

private:
	// calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		m_pCameraImpl->Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		m_pCameraImpl->Right = glm::normalize(glm::cross(m_pCameraImpl->Front, m_pCameraImpl->WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		m_pCameraImpl->Up = glm::normalize(glm::cross(m_pCameraImpl->Right, m_pCameraImpl->Front));
	}

private:
	CameraImpl *m_pCameraImpl = nullptr;
};
#endif