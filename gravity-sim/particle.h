#ifndef PARTICLE_H
#define PARTICLE_H

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <vector>

#include "util.h"
#include "camera.h"
#include "ObjLoader.hpp"
#include "ShaderLoader.h"

struct GLFWwindow;

struct ParticleSystemConfig
{
	GLFWwindow *Window;
	float CamVelocity;
	float CamSensitivity;
	float FoV;
	float AspectRatio;
	uint32_t NumParticles;
	GLfloat *ParticlePositions;
	GLfloat *ParticleMasses;
	std::string KernelSource;
	// TODO implement this...
	//GLfloat *ParticleVelocities;
	std::string ModelObjContents;
	std::string FragShader;
	std::string VertShader;
};

class ParticleSystem
{
private:
	// buffer IDs for cur, prev, & next positions (CL)
	cl_mem m_cl_buffs[3];
	cl_mem m_cl_mass;
	cl_mem m_cl_time;
	cl_program m_cl_program;
	cl_kernel m_cl_kernel;

	// current position buffer index
	uint8_t m_cur;

	GLint m_mvp_loc;
	GLint m_view_loc;
	GLint m_lightpos_loc;
	const GLint m_instance_index = 2;

	// instance positions buffer
	GLuint m_pos_instances[3];

	// timestep deltas for current and previous frame
	float m_delta1, m_delta2;
	float m_total;

	CL_Components m_CL;
	ParticleSystemConfig m_cfg;

	Camera m_cam;
	Model<3> m_model;
	Shader m_shader;
	glm::vec4 m_light_pos;
	
	bool m_error;
	std::string m_error_str;

	uint8_t prv_() const;
	uint8_t nxt_() const;
	uint8_t cur_() const;

	// Hide yo copy ctor
	ParticleSystem(const ParticleSystem& cfg) : m_cam(cfg.m_cfg)
	{};
public:
	ParticleSystem(CL_Components &comps, ParticleSystemConfig cfg);
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
		return m_error_str;
	};
};

#endif