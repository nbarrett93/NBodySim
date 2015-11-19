#include "InstSphereRenderer.h"
#include "ParticleShader.h"
#include "ReadFile.h"

#include <glm/gtc/type_ptr.hpp>

const std::string InstSphereRenderer::ModelPath = ".\\resources\\models\\sphere.obj";

InstSphereRenderer::InstSphereRenderer() :
Renderer(),
m_instance(0)
{
}

InstSphereRenderer::~InstSphereRenderer()
{
	Dispose();
}

void InstSphereRenderer::Load(const glm::vec4 *positions, size_t num_particles)
{
	m_num_particles = num_particles;

	std::string model_s = ReadFile(ModelPath, m_success);
	if (!m_success)
	{
		m_log += "[FAIL] Error reading model file\n";
		return;
	}

	Model<3> model = Model<3>::FromString(model_s);

	m_shader_.Load();

	if (!m_shader_.Success())
	{
		m_success = false;
		m_log += "Error loading particle shader: \n    " + m_shader_.ErrorText();
		return;
	}

	Renderer::Load(model, m_shader_);

	glGenBuffers(1, &m_instance);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_instance);
	glBufferData(GL_ARRAY_BUFFER, m_num_particles * sizeof(glm::vec4), positions, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(m_shader_.PositionLoc());
	glVertexAttribPointer(m_shader_.PositionLoc(), 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
	glVertexAttribDivisor(m_shader_.PositionLoc(), 1);

	glEnableVertexAttribArray(m_shader_.NormalLoc());
	glBindBuffer(GL_ARRAY_BUFFER, m_normals);
	glVertexAttribPointer(m_shader_.NormalLoc(), 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glEnableVertexAttribArray(m_shader_.ModelPosLoc());
	glBindBuffer(GL_ARRAY_BUFFER, m_vertices);
	glVertexAttribPointer(m_shader_.ModelPosLoc(), 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InstSphereRenderer::Dispose()
{
	if (m_instance)
		glDeleteBuffers(1, &m_instance);
	m_instance = 0;

	Renderer::Dispose();
}

void InstSphereRenderer::Render(const Camera &cam, const glm::vec4 &light_pos)
{
	glBindVertexArray(m_vao);
	glUseProgram(m_shader.Program());

	glm::mat4 view = cam.View();
	glm::mat4 view_persp = cam.Proj() * view;

	glUniformMatrix4fv(m_shader_.MVPULoc(), 1, GL_FALSE, glm::value_ptr(view_persp));
	glUniformMatrix4fv(m_shader_.ViewULoc(), 1, GL_FALSE, glm::value_ptr(view));
	glUniform4fv(m_shader_.LightPosULoc(), 1, glm::value_ptr(light_pos));

	glDrawElementsInstanced(GL_TRIANGLES, m_model.Indices.size(), GL_UNSIGNED_SHORT, (void*)0, m_num_particles);

	glUseProgram(0);
	glBindVertexArray(0);
}

GLuint InstSphereRenderer::BufferLoc() const
{
	return m_instance;
}