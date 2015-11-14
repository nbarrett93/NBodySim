#include "particle.h"

ParticleSystem::~ParticleSystem()
{
	if (m_gl_id[0] != 0)
	{
		glDeleteBuffers(3, m_gl_id);
	}
	memset(m_gl_id, 0, 3 * sizeof(m_gl_id[0]));
}

ParticleSystem::ParticleSystem(const CL_Components &comps) : 
	m_cur(1),
	m_CL(comps)
{
	glGenBuffers(3, m_gl_id);
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