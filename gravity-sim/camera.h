#ifndef CAMERA_H
#define CAMERA_H

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Camera
{
private:
	const float m_fov;
	const float m_aspect_ratio;
	const float m_near_clip = 0.1f;
	const float m_far_clip = 100.0f;

	glm::mat4 m_view_proj;

	glm::vec3 m_position;
	glm::vec3 m_looking;
	glm::vec3 m_up;
	glm::vec3 m_velocity;

public:
	Camera(float FoV, float aspect_ratio);

	void gen_mat();

	const glm::mat4& get_VP() const;

	void move_to(const glm::vec3 &loc);
	void face(const glm::vec3 &facing);

	void translate(const glm::vec3 &offset);
	void rotate(const glm::vec3 &axis, float angle);
};

#endif