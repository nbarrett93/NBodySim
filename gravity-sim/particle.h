#ifndef PARTICLE_H
#define PARTICLE_H

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <vector>

#include "util.h"
#include "camera.h"

struct ParticleSystemConfig
{
	float FoV;
	float AspectRatio;
	// TODO: more fields for shader files, kernel files, model, etc
};

class ParticleSystem
{
private:
	// buffer IDs for cur, prev, & next positions (GL & CL)
	GLuint m_gl_id[3];
	cl_mem m_cl_buffs[3];
	// buffer IDs for sphere vertices, normals, & indices (GL only)
	GLuint m_sphere_v,
		m_sphere_i,
		m_sphere_n;
	// buffer IDs for color vec (GL & CL)
	GLuint m_gl_color;
	cl_mem m_cl_color;

	// current position buffer index
	uint8_t m_cur;
	uint32_t m_num_particles;

	// timestep deltas for current and previous frame
	float m_delta1, m_delta2;

	CL_Components m_CL;
	Camera m_cam;

	// cam config doesn't matter here; we won't be using this
	ParticleSystem(const ParticleSystem&) : m_cam(0.1f, 4.0f / 3.0f)
	{};

	uint8_t prv_() const;
	uint8_t nxt_() const;
	uint8_t cur_() const;

	void dummy_setup();

public:
	ParticleSystem(const CL_Components &comps, ParticleSystemConfig cfg);
	ParticleSystem(CL_Components &&comps, ParticleSystemConfig cfg);
	~ParticleSystem();

	// TODO: loader for initial state

	void update_cam();
	void run_CL();
	void draw();
	void delta(float dt);
};

#endif