#ifndef PARTICLE_H
#define PARTICLE_H

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <vector>

#include "SystemCL.h"
#include "ParticleShader.h"
#include "BounceKernel.h"
#include "InstSphereRenderer.h"
#include "camera.h"
#include "ObjLoader.hpp"
#include "CamSettings.h"

#include "SystemConfig.h"

class ParticleSystem
{
private:
	// timestep deltas for current and previous frame
	float m_delta1, m_delta2;
	float m_total;

	SystemCL m_CL;
	ParticleSystemConfig m_cfg;

	Camera m_cam;
	InstSphereRenderer m_renderer;
	BounceKernel m_kernel;
	glm::vec4 m_light_pos;
	
	bool m_error;
	std::string m_log;

	// Hide yo copy ctor
	ParticleSystem(const ParticleSystem& rhs) : m_cam(rhs.m_cam), m_kernel(rhs.m_CL)
	{};
public:
	ParticleSystem(SystemCL &&comps, ParticleSystemConfig cfg);
	~ParticleSystem();

	void Init();

	void HandleMouse(double x, double y);
	void HandleKey(int key, int action);
	void HandleButton(int button, int action);

	void update(float dt);
	void draw();

	bool good() const { return !m_error; };
	std::string err_log() const
	{
		return m_log;
	};
};

#endif