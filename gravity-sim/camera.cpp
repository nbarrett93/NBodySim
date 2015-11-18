#include "camera.h"

#include "particle.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <GLFW/glfw3.h>

Camera::Camera(const ParticleSystemConfig &cfg, float near_clip, float far_clip) :
	m_position(0.0f, 2.0f, 4.0f),
	m_near_clip(near_clip),
	m_far_clip(far_clip),
	m_proj(glm::perspective(cfg.FoV, cfg.AspectRatio, near_clip, far_clip)),
	m_sens(cfg.CamSensitivity),
	m_vel(cfg.CamVelocity),
	m_up(0.0, 1.0, 0.0),
	m_right(1.0, 0.0, 0.0),
	m_left_down(false),
	m_right_down(false),
	m_space_down(false),
	m_dx(0),
	m_dy(0),
	m_xpos(0),
	m_ypos(0),
	m_config(cfg)
{
}

const glm::mat4 Camera::View() const
{
	glm::quat quat = glm::rotation(glm::cross(m_right, m_up), glm::vec3(0.0, 0.0, 1.0));
	return glm::mat4_cast(quat) * glm::translate(m_position * -1.0f);
}

glm::vec3 Camera::Up() const
{
	return m_up;
}

glm::vec3 Camera::Right() const
{
	return m_right;
}

bool Camera::HandleKey(int key, int action)
{
	switch (key)
	{
	case GLFW_KEY_SPACE:
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
			m_space_down = true;
		if (action == GLFW_RELEASE)
			m_space_down = false;
		return true;
		break;
	case GLFW_KEY_ENTER:
		// reset camera orientation
		// this line is from the old method of storing rotation as a quat
		//m_quat = glm::quat(1.0, 0.0, 0.0, 0.0);
		m_up = glm::vec3(0.0, 1.0, 0.0);
		m_right = glm::vec3(1.0, 0.0, 0.0);
		return true;
	default:
		return false;
		break;
	}

	return true;
}

bool Camera::HandleMouse(double xpos, double ypos)
{
	if (m_left_down || m_right_down)
	{
		m_dx += xpos - m_xpos;
		m_dy += ypos - m_ypos;
	}

	m_xpos = xpos;
	m_ypos = ypos;

	return true;
}

bool Camera::HandleButton(int button, int action)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS)
		{
			// hide the cursor
			m_left_down = true;
			glfwSetInputMode(m_config.Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			// show the cursor
			m_left_down = false;
			glfwSetInputMode(m_config.Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		return true;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (action == GLFW_PRESS)
		{
			// hide the cursor
			m_right_down = true;
			glfwSetInputMode(m_config.Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			// show the cursor
			m_right_down = false;
			glfwSetInputMode(m_config.Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		return true;
		break;
	default:
		return false;
		break;
	}

	return true;
}

void Camera::Update()
{
	glm::vec3 pos_vec(m_dx, m_dy, 0.0);
	float len = pos_vec.length();

	// if we dont have any offset, just return
	if (len < 0.00001)
		return;

	if (m_right_down)
	{
		if (m_space_down)
		{
			// go 'up'
			m_position -= m_up * (float)m_dy * m_config.CamVelocity;
		}
		else
		{
			// go 'forward'
			glm::vec3 dir = glm::normalize(glm::cross(m_up, m_right));
			m_position -= dir * (float)m_dy * m_config.CamVelocity;
		}

		m_position -= m_right * (float)m_dx * m_config.CamVelocity;

		m_dx = 0;
		m_dy = 0;
	}
	else if (m_left_down)
	{
		glm::vec3 p_hat = pos_vec / len;

		// axis of rotation
		glm::vec3 r = glm::cross(glm::cross(m_right, m_up), p_hat);

		// theta gives angle of rotation
		float theta = m_config.CamSensitivity * len;

		glm::quat q = glm::angleAxis(theta, r);

		m_right = glm::normalize(q * m_right);
		m_up = glm::normalize(q * m_up);

		m_dx = 0;
		m_dy = 0;
	}
}