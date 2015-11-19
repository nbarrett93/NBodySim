#include "particle.h"
#include "ObjLoader.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

ParticleSystem::~ParticleSystem()
{
	m_kernel.Dispose();
	m_CL.Dispose();
}

ParticleSystem::ParticleSystem(SystemCL &&comps, ParticleSystemConfig cfg) :
	m_CL(std::move(comps)),
	m_kernel(m_CL),
	m_cam(cfg.CamSettings),
	m_delta1(0.033f),
	m_delta2(0.033f),
	m_model(Model<3>::FromString(cfg.ModelObjContents)),
	m_error(false),
	m_cfg(cfg),
	m_light_pos(0.0, 100.0, 0.0, 0.0),
	m_total(0.0)
{
}

void ParticleSystem::Init()
{
	// Create the instanced sphere renderer here

	m_model.MakeVAO();

	glBindVertexArray(m_model.VAO);

	glGenBuffers(m_cfg.NumParticles, m_pos_instances);
	glBindBuffer(GL_ARRAY_BUFFER, m_pos_instances[0]);

	glBufferData(GL_ARRAY_BUFFER, m_cfg.NumParticles * 4 * sizeof(GLfloat), m_cfg.ParticlePositions, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribDivisor(2, 1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_shader.Load();
	if (!m_shader.Success())
	{
		m_log += "[SHADER] " + m_shader.ErrorText();
		m_error = true;
		return;
	}

	m_kernel.Load(gl_pos_buff, m_cfg.NumParticles);
}

void ParticleSystem::update(float dt)
{
	m_delta2 = m_delta1;
	m_delta1 = dt;
	m_total += dt;

	m_cam.Update();

	m_kernel.Run(m_total);
}

void ParticleSystem::draw()
{
	// call sphere's rendering method. remove all of this crap.

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_shader.Program());

	glBindVertexArray(m_model.VAO);

	glm::mat4 view = m_cam.View();
	glm::mat4 view_persp = m_cam.Proj() * view;

	glUniformMatrix4fv(m_shader.MVPULoc(), 1, GL_FALSE, glm::value_ptr(view_persp));
	glUniformMatrix4fv(m_shader.ViewULoc(), 1, GL_FALSE, glm::value_ptr(view));
	glUniform4fv(m_shader.LightPosULoc(), 1, glm::value_ptr(m_light_pos));

	glDrawElementsInstanced(GL_TRIANGLES, m_model.Indices.size(), GL_UNSIGNED_SHORT, (void*)0, m_cfg.NumParticles);

	glBindVertexArray(0);

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