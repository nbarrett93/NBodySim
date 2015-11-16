#include "particle.h"
#include "ObjLoader.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLfloat dummy_positions[2 * 4] = 
{
	1.0,  0.0, 0.0,
	-1.0, 0.0, 0.0
};

ParticleSystem::~ParticleSystem()
{
	// should probably do something about this situation...
}

ParticleSystem::ParticleSystem(CL_Components &&comps, ParticleSystemConfig cfg) :
	m_cur(1),
	m_CL(std::forward<CL_Components>(comps)),
	m_cam(cfg.FoV, cfg.AspectRatio),
	m_delta1(0.033f),
	m_delta2(0.033f),
	m_num_particles(2),
	m_sphere_model(Model<3>::FromString(cfg.SphereObjContents)),
	m_error(false),
	m_cfg(cfg)
{
	m_sphere_vao = m_sphere_model.MakeVAO();

	m_vert_shader = glCreateShader(GL_VERTEX_SHADER);

	const char* src = cfg.VertShader.c_str();

	glShaderSource(m_vert_shader, 1, &src, NULL);
	glCompileShader(m_vert_shader);
	GLint succ;
	glGetShaderiv(m_vert_shader, GL_COMPILE_STATUS, &succ);

	if (succ == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(m_vert_shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(m_vert_shader, maxLength, &maxLength, &errorLog[0]);

		m_error_str = std::string(errorLog.begin(), errorLog.end());

		m_error = true;
		glDeleteShader(m_vert_shader);
		return;
	}

	m_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	src = cfg.FragShader.c_str();

	glShaderSource(m_frag_shader, 1, &src, NULL);
	glCompileShader(m_frag_shader);
	glGetShaderiv(m_frag_shader, GL_COMPILE_STATUS, &succ);

	if (succ == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(m_vert_shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(m_vert_shader, maxLength, &maxLength, &errorLog[0]);

		m_error_str = std::string(errorLog.begin(), errorLog.end());

		m_error = true;
		glDeleteShader(m_frag_shader);
		glDeleteShader(m_vert_shader);
		return;
	}

	m_shader_program = glCreateProgram();

	glAttachShader(m_shader_program, m_vert_shader);
	glAttachShader(m_shader_program, m_frag_shader);

	glLinkProgram(m_shader_program);

	glGetProgramiv(m_shader_program, GL_LINK_STATUS, &succ);
	if (succ == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(m_shader_program, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(m_shader_program, maxLength, &maxLength, &infoLog[0]);
		m_error_str = std::string(infoLog.begin(), infoLog.end());

		glDeleteProgram(m_shader_program);

		glDeleteShader(m_vert_shader);
		glDeleteShader(m_frag_shader);

		m_error = true;
		glDeleteProgram(m_shader_program);
		return;
	}

	m_mvp_loc = glGetUniformLocation(m_shader_program, "mvp_matrix");
	
	glDetachShader(m_shader_program, m_vert_shader);
	glDeleteShader(m_vert_shader);

	glDetachShader(m_shader_program, m_frag_shader);
	glDeleteShader(m_frag_shader);
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

	glUseProgram(m_shader_program);

	glBindVertexArray(m_sphere_vao);

	// just fixed camera position for now...
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 8.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 view_persp = glm::perspective(m_cfg.FoV, m_cfg.AspectRatio, 0.1f, 100.0f) * view;

	glUniformMatrix4fv(m_mvp_loc, 1, GL_FALSE, glm::value_ptr(view_persp));

	glDrawElements(GL_TRIANGLES, m_sphere_model.Indices.size(), GL_UNSIGNED_SHORT, (void*)0);

	glBindVertexArray(0);

	glFinish();
}
