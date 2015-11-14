#include "camera.h"

Camera::Camera(float FoV, float aspect_ratio) :
	m_aspect_ratio(aspect_ratio),
	m_fov(FoV),
	m_looking(0.0f, 0.0f, 0.0f),
	m_up(0.0f, 1.0f, 0.0f),
	m_position(0.0f, 0.0f, -1.0f),
	m_velocity(0.0f, 0.0f, 0.0f)
{
}

void Camera::gen_mat()
{

}

const glm::mat4& Camera::get_VP() const
{ return m_view_proj; }
