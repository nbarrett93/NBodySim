#ifndef PARTICLE_H
#define PARTICLE_H

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <vector>

#include "util.h"
#include "camera.h"
#include "ObjLoader.hpp"

struct ParticleSystemConfig
{
	float FoV;
	float AspectRatio;
	std::string SphereObjContents;
	std::string FragShader;
	std::string VertShader;
	// TODO: more fields for shader files, kernel files, model, etc
};

class ParticleSystem
{
private:
	// buffer IDs for cur, prev, & next positions (CL)
	cl_mem m_cl_buffs[3];
	cl_mem m_cl_color;

	GLuint m_vert_shader,
		m_frag_shader,
		m_shader_program;

	GLint m_mvp_loc;

	// current position buffer index
	uint8_t m_cur;
	uint32_t m_num_particles;

	// timestep deltas for current and previous frame
	float m_delta1, m_delta2;

	CL_Components m_CL;
	Camera m_cam;
	ParticleSystemConfig m_cfg;
	std::string m_error_str;

	Model<3> m_sphere_model;
	GLuint m_sphere_vao;

	// cam config doesn't matter here; we won't be using this
	ParticleSystem(const ParticleSystem&) : m_cam(0.1f, 4.0f / 3.0f)
	{};

	uint8_t prv_() const;
	uint8_t nxt_() const;
	uint8_t cur_() const;

	bool m_error;

public:
	ParticleSystem(CL_Components &&comps, ParticleSystemConfig cfg);
	~ParticleSystem();

	// TODO: loader for initial state

	void update_cam();
	void run_CL();
	void draw();
	void delta(float dt);
	bool good() const { return !m_error; };
	std::string err_log() const
	{
		return m_error_str;
	};
};

#endif