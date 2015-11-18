#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

struct ParticleSystemConfig;

class Camera
{
private:
	const float m_near_clip;
	const float m_far_clip;

	const float m_sens;
	const float m_vel;

	const glm::mat4 m_proj;

	glm::vec3 m_position;
	glm::vec3 m_up;
	glm::vec3 m_right;
	//glm::dquat m_quat;

	bool m_left_down;
	bool m_right_down;
	bool m_space_down;

	double m_xpos, m_ypos;
	double m_dx, m_dy;

	const ParticleSystemConfig& m_config;

public:
	Camera(const ParticleSystemConfig &cfg, float near_clip = 0.1f, float far_clip = 100.0f);

	const glm::mat4 View() const;
	const glm::mat4& Proj() const
	{
		return m_proj;
	};

	glm::vec3 Up() const;
	glm::vec3 Right() const;

	void Update();

	bool HandleKey(int key, int action);
	bool HandleMouse(double xpos, double ypos);
	bool HandleButton(int button, int action);
};

#endif