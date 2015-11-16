#include "particle.h"
#include "ObjLoader.hpp"

GLfloat dummy_positions[2 * 4] = 
{
	1.0,  0.0, 0.0,
	-1.0, 0.0, 0.0
};

ParticleSystem::~ParticleSystem()
{
	
}

ParticleSystem::ParticleSystem(CL_Components &&comps, ParticleSystemConfig cfg) :
	m_cur(1),
	m_CL(std::move(comps)),
	m_cam(cfg.FoV, cfg.AspectRatio),
	m_delta1(0.033f),
	m_delta2(0.033f),
	m_num_particles(2),
	m_sphere_model(Model<3>::FromString(cfg.SphereObjContents))
{
	m_sphere_vao = m_sphere_model.MakeVAO();
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

void ParticleSystem::delta(float dt)
{
	m_delta2 = m_delta1;
	m_delta1 = dt;
}

void ParticleSystem::update_cam()
{
	// TODO: update camera position dep upon either user input or the CL simulation output
}

void ParticleSystem::run_CL()
{
	cl_int cl_ret;
	cl_event cl_ev[2];

	/************************ acquire buffers *************************************/
	cl_ret = clEnqueueAcquireGLObjects(m_CL.queue, 3, m_cl_buffs, 0, NULL, &cl_ev[0]);
	if (cl_ret != CL_SUCCESS)
	{
		// uh-oh. TODO
	}

	cl_ret = clEnqueueAcquireGLObjects(m_CL.queue, 1, &m_cl_color, 0, NULL, &cl_ev[1]);
	if (cl_ret != CL_SUCCESS)
	{
		// uh-oh. TODO
	}
	clWaitForEvents(2, cl_ev);

	/************************** run kernel ****************************************/



	// update cam while the kernel is running
	update_cam();
	// and then wait for the kernel (woo task-parallelism)


	/************************ release buffers *************************************/
	cl_ret = clEnqueueReleaseGLObjects(m_CL.queue, 3, m_cl_buffs, 0, NULL, &cl_ev[0]);
	if (cl_ret != CL_SUCCESS)
	{
		// uh-oh. TODO
	}

	cl_ret = clEnqueueReleaseGLObjects(m_CL.queue, 1, &m_cl_color, 0, NULL, &cl_ev[1]);
	if (cl_ret != CL_SUCCESS)
	{
		// uh-oh. TODO
	}
	clWaitForEvents(2, cl_ev);

	// quite possibly superfluous
	clFinish(m_CL.queue);
}

void ParticleSystem::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// do drawing
	//glDrawElementsInstanced(GL_TRIANGLES, m_sphere_num_indices, GL_UNSIGNED_SHORT, NULL, 2);

	glFinish();
}

// This needs to be extracted into a class
void ParticleSystem::dummy_setup()
{
	/***************** Offsets ******************/

	GLfloat dummy_data[4 * 4] =
	{    /*    Positions              Colors    */
		 1.0, 0.0, 0.0, 0.0,    1.0, 0.0, 1.0, 1.0,
		-1.0, 0.0, 0.0, 0.0,    0.0, 0.0, 1.0, 1.0
	};

	//glBindBuffer(GL_ARRAY_BUFFER, m_gl_id[1]);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), dummy_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//glBindBuffer(GL_ARRAY_BUFFER, m_gl_id[1]);
	// offset & stride for the positions
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	// offset & stride for the colors
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)4);
	glVertexAttribDivisor(0, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/***************** Sphere *****************/

	auto sphere = Model<4>::FromCStr("Hello!");

	//glBindVertexArray(m_sphere_v);
	//glBufferData(GL_ARRAY_BUFFER, m_sphere_vert_sz, m_sphere_vertices, GL_STATIC_DRAW);
	glBindVertexArray(0);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sphere_i);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_sphere_num_indices * sizeof(unsigned short), m_sphere_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}