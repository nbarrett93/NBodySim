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

void InstSphereRenderer::Load(const glm::vec4 *positions, size_t num_particles, ShaderMgr &sm)
{
	m_num_particles = num_particles;

	std::string model_s = ReadFile(ModelPath, m_success);
	if (!m_success)
	{
		m_log += "[FAIL] Error reading model file\n";
		return;
	}

	Model<3> model = Model<3>::FromString(model_s);

	std::unique_ptr<ParticleShader> shader = std::make_unique<ParticleShader>();

	shader->Load(sm);

	if (!shader->Success())
	{
		m_success = false;
		m_log += "Error loading particle shader: \n    " + shader->ErrorText();
		return;
	}

	Renderer::Load(model, std::move(shader));

	glGenBuffers(1, &m_instance);

	glBindVertexArray(m_vao);

	// borrow shader
	const ParticleShader *shader_ = static_cast<ParticleShader*>(m_shader.get());

	glBindBuffer(GL_ARRAY_BUFFER, m_instance);
	glBufferData(GL_ARRAY_BUFFER, m_num_particles * sizeof(glm::vec4), positions, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(shader_->PositionLoc());
	glVertexAttribPointer(shader_->PositionLoc(), 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
	glVertexAttribDivisor(shader_->PositionLoc(), 1);

	glEnableVertexAttribArray(shader_->NormalLoc());
	glBindBuffer(GL_ARRAY_BUFFER, m_normals);
	glVertexAttribPointer(shader_->NormalLoc(), 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glEnableVertexAttribArray(shader_->ModelPosLoc());
	glBindBuffer(GL_ARRAY_BUFFER, m_vertices);
	glVertexAttribPointer(shader_->ModelPosLoc(), 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);

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
	glUseProgram(m_shader->Program());

	const ParticleShader *shader = static_cast<ParticleShader*>(m_shader.get());

	glm::mat4 view = cam.View();
	glm::mat4 view_persp = cam.Proj() * view;

	glUniformMatrix4fv(shader->MVPULoc(), 1, GL_FALSE, glm::value_ptr(view_persp));
	glUniformMatrix4fv(shader->ViewULoc(), 1, GL_FALSE, glm::value_ptr(view));
	glUniform4fv(shader->LightPosULoc(), 1, glm::value_ptr(light_pos));

	glDrawElementsInstanced(GL_TRIANGLES, m_model.Indices.size(), GL_UNSIGNED_SHORT, (void*)0, m_num_particles);

	glUseProgram(0);
	glBindVertexArray(0);
}

GLuint InstSphereRenderer::BufferLoc() const
{
	return m_instance;
}