#ifndef PARTICLE_H
#define PARTICLE_H

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <vector>

#include "util.h"

class ParticleSystemConfig
{
private:
public:
};

class ParticleSystem
{
private:
	// buffer IDs for cur, prev, & next positions
	GLuint m_gl_id[3];
	cl_mem m_cl_buffs[3];
	uint8_t m_cur;
	CL_Components m_CL;

	ParticleSystem(const ParticleSystem&)
	{};

	uint8_t prv_() const;
	uint8_t nxt_() const;
	uint8_t cur_() const;

public:
	ParticleSystem(const CL_Components &comps);
	~ParticleSystem();

	void run_CL();

	void draw();
};

#endif