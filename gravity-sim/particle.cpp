#include "particle.h"
#include "ObjLoader.hpp"
#include "ShaderLoader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

ParticleSystem::~ParticleSystem()
{
	// TODO BAD!!!
	return;

	if (m_cl_program)
		clReleaseProgram(m_cl_program);

	for (uint32_t i = 0; i < m_cfg.NumParticles; ++i)
		if (m_cl_buffs[i])
			clReleaseMemObject(m_cl_buffs[i]);

	if (m_cl_mass)
		clReleaseMemObject(m_cl_mass);

	m_CL.dispose();

	glDeleteBuffers(3, m_pos_instances);
}

ParticleSystem::ParticleSystem(CL_Components &comps, ParticleSystemConfig cfg) :
	m_cur(1),
	m_CL(comps),
	m_cam(cfg),
	m_delta1(0.033f),
	m_delta2(0.033f),
	m_model(Model<3>::FromString(cfg.ModelObjContents)),
	m_error(false),
	m_cfg(cfg),
	m_light_pos(0.0, 100.0, 0.0, 0.0),
	m_cl_mass(0),
	m_total(0.0)
{
	for (uint32_t i = 0; i < m_cfg.NumParticles; ++i)
		m_cl_buffs[i] = 0;
}

void ParticleSystem::Init()
{
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

	m_shader.LoadProgram(m_cfg.VertShader, m_cfg.FragShader);

	if (m_shader.Error())
	{
		m_error = true;
		m_error_str = m_shader.ErrorText;
		return;
	}

	m_mvp_loc = glGetUniformLocation(m_shader.Program, "mvp_matrix");
	m_view_loc = glGetUniformLocation(m_shader.Program, "v_matrix");
	m_lightpos_loc = glGetUniformLocation(m_shader.Program, "light_pos_world");

	// get our OpenCL objects

	cl_int clErr;
	/*for (uint32_t i = 0; i < m_cfg.NumParticles; ++i)
	{
		m_cl_buffs[i] = clCreateFromGLBuffer(m_CL.context, CL_MEM_READ_WRITE, m_pos_instances[i], &clErr);
		if (clErr)
		{
			m_error = true;
			m_error_str = "Unable to acquire GL buffer";
			return;
		}
	}*/

	// only using first buffer ATM for testing
	m_cl_buffs[0] = clCreateFromGLBuffer(m_CL.context, CL_MEM_READ_WRITE, m_pos_instances[0], &clErr);
	if (clErr)
	{
		m_error = true;
		m_error_str = "Unable to acquire GL buffer [Position]";
		return;
	}

	m_cl_time = clCreateBuffer(m_CL.context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, sizeof(GLfloat), NULL, &clErr);
	if (clErr)
	{
		m_error = true;
		m_error_str = "Unable to create buffer [Time]";
		return;
	}

	// not using mass for testing the system
	/*m_cl_mass = clCreateBuffer(m_CL.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
		m_cfg.NumParticles * sizeof(GLfloat), m_cfg.ParticleMasses, &clErr);
	if (clErr)
	{
		m_error = true;
		m_error_str = "Unable to acquire buffer [Mass]";
		return;
	}*/

	// get the OpenCL kernel
	// a test kernel for now
	const char *str = m_cfg.KernelSource.c_str();
	m_cl_program = clCreateProgramWithSource(m_CL.context, 1, &str, NULL, &clErr);
	if (clErr)
	{
		m_error = true;
		m_error_str = "Unable to load kernel.";
		return;
	}

	clErr = clBuildProgram(m_cl_program, 1, &m_CL.device, NULL, NULL, NULL);
	if (CL_SUCCESS != clErr)
	{
		m_error = true;
		m_error_str = "Unable to build kernel.";
		return;
	}

	m_cl_kernel = clCreateKernel(m_cl_program, "calc_next", &clErr);
	if (CL_SUCCESS != clErr)
	{
		m_error = true;
		m_error_str = "Unable to instantiate kernel.";
		return;
	}
}

uint8_t ParticleSystem::prv_() const
{
	switch (m_cur)
	{
	case 0: return 2;
	case 1: return 0;
	case 2: return 1;
	default: break;
	}
	return 0;
}

uint8_t ParticleSystem::nxt_() const
{
	switch (m_cur)
	{
	case 0: return 1;
	case 1: return 2;
	case 2: return 0;
	default: break;
	}
	return 0;
}

uint8_t ParticleSystem::cur_() const
{
	return m_cur;
}

void ParticleSystem::update(float dt)
{
	m_delta2 = m_delta1;
	m_delta1 = dt;

	m_total += dt;

	m_cam.Update();

	cl_int cl_ret;
	cl_event cl_ev[2];

	/************************ acquire buffers *************************************/

	//cl_ret = clEnqueueAcquireGLObjects(m_CL.queue, 3, m_cl_buffs, 0, NULL, &cl_ev[0]);
	cl_ret = clEnqueueAcquireGLObjects(m_CL.queue, 1, &m_cl_buffs[0], 0, NULL, cl_ev);
	if (cl_ret != CL_SUCCESS)
	{
		// uh-oh. TODO
		m_error = true;
		m_error_str = "Error acquiring GL object";
		return;
	}

	//cl_ret = clEnqueueAcquireGLObjects(m_CL.queue, 1, &m_cl_color, 0, NULL, &cl_ev[1]);
	//if (cl_ret != CL_SUCCESS)
	//{
		// uh-oh. TODO
	//}
	//clWaitForEvents(2, cl_ev);

	clEnqueueWriteBuffer(m_CL.queue, m_cl_time, CL_FALSE, 0, 4, &m_total, NULL, NULL, &cl_ev[1]);

	clWaitForEvents(2, cl_ev);
	clReleaseEvent(cl_ev[0]);
	clReleaseEvent(cl_ev[1]);

	/************************** run kernel ****************************************/

	cl_ret = clSetKernelArg(m_cl_kernel, 0, sizeof(cl_mem), &m_cl_buffs[0]);
	if (cl_ret != CL_SUCCESS)
	{
		m_error_str = "Error setting argument to kernel";
		m_error = true;
	}
	else
	{
		cl_ret = clSetKernelArg(m_cl_kernel, 1, sizeof(cl_mem), &m_cl_time);
		if (cl_ret != CL_SUCCESS)
		{
			m_error_str = "Error setting argument to kernel";
			m_error = true;
		}
		else
		{
			size_t work_sz = m_cfg.NumParticles;
			cl_ret = clEnqueueNDRangeKernel(m_CL.queue, m_cl_kernel, 1, NULL, &work_sz, NULL, 0, NULL, cl_ev);
			if (cl_ret != CL_SUCCESS)
			{
				m_error_str = "Error queueing kernel.";
				m_error = true;
			}
		}
	}

	clWaitForEvents(1, cl_ev);
	clReleaseEvent(cl_ev[0]);

	/************************ release buffers *************************************/

	//cl_ret = clEnqueueReleaseGLObjects(m_CL.queue, 3, m_cl_buffs, 0, NULL, &cl_ev[0]);
	cl_ret = clEnqueueReleaseGLObjects(m_CL.queue, 1, &m_cl_buffs[0], 0, NULL, cl_ev);
	if (cl_ret != CL_SUCCESS)
	{
		// uh-oh. TODO
		m_error = true;
		m_error_str = "Error releasing GL object";
		return;
	}

	//cl_ret = clEnqueueReleaseGLObjects(m_CL.queue, 1, &m_cl_color, 0, NULL, &cl_ev[1]);
	//if (cl_ret != CL_SUCCESS)
	//{
		// uh-oh. TODO
	//}
	//clWaitForEvents(2, cl_ev);
	clWaitForEvents(1, cl_ev);
	clReleaseEvent(cl_ev[0]);

	// possibly superfluous
	clFlush(m_CL.queue);
}

void ParticleSystem::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_shader.Program);

	glBindVertexArray(m_model.VAO);

	glm::mat4 view = m_cam.View();
	glm::mat4 view_persp = m_cam.Proj() * view;

	glUniformMatrix4fv(m_mvp_loc, 1, GL_FALSE, glm::value_ptr(view_persp));
	glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view));
	glUniform4fv(m_lightpos_loc, 1, glm::value_ptr(m_light_pos));

	glDrawElementsInstanced(GL_TRIANGLES, m_model.Indices.size(), GL_UNSIGNED_SHORT, (void*)0, m_cfg.NumParticles);

	glBindVertexArray(0);

	// sync up so that we can run OpenCL
	glFinish();
}

void ParticleSystem::HandleMouse(double x, double y)
{
	// hard-code in the camera class for now
	// TODO make this more generic so a menu can be added
	m_cam.HandleMouse(x, y);
}

void ParticleSystem::HandleKey(int key, int action)
{
	m_cam.HandleKey(key, action);
}

void ParticleSystem::HandleButton(int button, int action)
{
	// same as mouse. TODO
	m_cam.HandleButton(button, action);
}