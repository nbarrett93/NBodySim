#include "particle.h"
#include "ObjLoader.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>

ParticleSystem::~ParticleSystem()
{
	m_kernel->Dispose();
	m_CL.Dispose();
}

ParticleSystem::ParticleSystem(SystemCL &&comps, ParticleSystemConfig cfg) :
	m_CL(std::move(comps)),
	m_kernel(std::make_unique<BounceKernel>(m_CL)),
	m_renderer(std::make_unique<InstSphereRenderer>()),
	m_cam(cfg.CamSettings),
	m_delta1(0.033f),
	m_delta2(0.033f),
	m_error(false),
	m_cfg(cfg),
	m_light_pos(0.0, 4.0, -4.0, 0.0),
	m_total(0.0)
{
}

void ParticleSystem::Init()
{
    if (!m_shaderMgr.LoadDir(".\\resources\\shaders"))
    {
        m_error = true;
        m_log += "Error initializing shaderMgr: " + m_shaderMgr.ErrorLog() + "\n";
        return;
    }

	static_cast<InstSphereRenderer*>(m_renderer.get())->Load(
		m_cfg.ParticlePositions, 
		m_cfg.NumParticles,
        m_shaderMgr);

	if (!m_renderer->Success())
	{
		m_error = true;
		m_log += "Renderer load error: " + m_renderer->ErrorText() + "\n";
		return;
	}

	// this is a bad solution. Need to provide a way to abstract out the instanced nature of the
	// rendered object & link it with the CL kernel
	static_cast<BounceKernel*>(m_kernel.get())->Load(
		static_cast<InstSphereRenderer*>(m_renderer.get())->BufferLoc(), 
		m_cfg.NumParticles);

	if (!m_kernel->Success())
	{
		m_error = true;
		m_log += "Kernel load error: " + m_renderer->ErrorText() + "\n";
		return;
	}
}

void ParticleSystem::update(float dt)
{
	m_delta2 = m_delta1;
	m_delta1 = dt;
	m_total += dt;

	m_cam.Update(dt);

	static_cast<BounceKernel*>(m_kernel.get())->Run(m_total);

	if (!m_kernel->Success())
	{
		m_error = true;
		m_log += "Error updating system: " + m_kernel->ErrorText() + "\n";
	}
}

void ParticleSystem::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static_cast<InstSphereRenderer*>(m_renderer.get())->Render(m_cam, m_light_pos);

	if (!m_renderer->Success())
	{
		m_error = true;
		m_log += "Renderer error: " + m_renderer->ErrorText() + "\n";
		return;
	}

	// sync up so that we can run OpenCL
	glFinish();
}

void ParticleSystem::HandleMouse(double x, double y)
{
	m_cam.HandleMouse(x, y);
}

void ParticleSystem::HandleKey(int key, int action)
{
	m_cam.HandleKey(key, action);
}

void ParticleSystem::HandleButton(int button, int action)
{
	m_cam.HandleButton(button, action);
}