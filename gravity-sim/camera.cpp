#include "camera.h"

#include "particle.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <GLFW/glfw3.h>

Camera::Camera(const CameraSettings &cfg, float near_clip, float far_clip) :
	m_position(0.0f, 2.0f, 4.0f),
	m_near_clip(near_clip),
	m_far_clip(far_clip),
	m_proj(glm::perspective(cfg.FoV, cfg.AspectRatio, near_clip, far_clip)),
	//m_quat(0.0, 0.0, 0.0, 0.0),
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
	return glm::mat4_cast((glm::fquat)glm::inverse(m_quat)) * glm::translate(m_position * -1.0f);
}

const glm::mat4& Camera::Proj() const
{
	return m_proj;
}

glm::dvec3 Camera::Up() const
{
	return glm::normalize(m_quat * glm::dvec3(0.0, 1.0, 0.0));
}

glm::dvec3 Camera::Right() const
{
	return glm::normalize(m_quat * glm::dvec3(1.0, 0.0, 0.0));;
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
		m_quat = glm::dquat();
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

void Camera::Update(float dt)
{
	double len = glm::length(glm::dvec2(m_dx, m_dy));

	// if we dont have any offset, just return
	if (len < 0.00001)
		return;

	glm::dvec3 pos_vec = glm::normalize(glm::vec3(m_dx, m_dy, 0.0));

	// (0,1,0)
	glm::dvec3 right = Right();
	// (1,0,0)
	glm::dvec3 up = Up();
	// (0,0,-1)
	glm::dvec3 forward = glm::normalize(glm::cross(up, right));

	if (m_right_down)
	{
		if (m_space_down)
		{
			// go 'up'
			m_position -= up * m_dy * m_config.CamVelocity;
		}
		else
		{
			// go 'forward'
			m_position -= forward * m_dy * m_config.CamVelocity;
		}

		m_position -= right * m_dx * m_config.CamVelocity;

		m_dx = 0;
		m_dy = 0;
	}
	else if (m_left_down)
	{
		glm::dquat pitch_rot(1.0, 0.0, 0.0, 0.0);

		double dp = glm::dot(forward, glm::dvec3(0.0, 1.0, 0.0));
		double pitch = glm::acos(dp) - (glm::pi<double>() / 2.0);

		double upper_pitch_bound = 0.95 * glm::pi<double>() / 2.0;
		double lower_pitch_bound = -1.0 * upper_pitch_bound;

		if (!((m_dy > 0.0 && pitch > upper_pitch_bound) || (m_dy < 0.0 && pitch < lower_pitch_bound)))
			pitch_rot = glm::angleAxis(-1.0 * m_dy * m_config.CamSensitivity, right);

		glm::dquat yaw_rot = glm::angleAxis(m_dx * m_config.CamSensitivity, glm::dvec3(0.0, 1.0, 0.0));

		m_quat = glm::normalize(yaw_rot * pitch_rot * m_quat);

		m_dx = 0;
		m_dy = 0;
	}
}