#include "Renderer.h"

Renderer::Renderer() :
m_vertices(0),
m_vao(0),
m_normals(0),
m_texcoords(0),
m_indices(0),
m_success(true),
m_log("")
{
}

Renderer::~Renderer()
{
	Dispose();
}

void Renderer::Load(Model<3> &model, ShaderBase &shader)
{
	m_model = model;
	m_shader = shader;

	glGenVertexArrays(1, &m_vao);

	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vertices);
	glGenBuffers(1, &m_normals);
	glGenBuffers(1, &m_indices);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertices);
	glBufferData(GL_ARRAY_BUFFER, m_model.Vertices.size() * sizeof(m_model.Vertices[0]), &m_model.Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_normals);
	glBufferData(GL_ARRAY_BUFFER, m_model.Normals.size() * sizeof(m_model.Normals[0]), &m_model.Normals[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_model.Indices.size() * sizeof(m_model.Normals[0]), &m_model.Indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderer::Dispose()
{
	if (m_vao)
		glDeleteVertexArrays(1, &m_vao);
	m_vao = 0;

	if (m_vertices)
		glDeleteBuffers(1, &m_vertices);
	m_vertices = 0;

	if (m_normals)
		glDeleteBuffers(1, &m_normals);
	m_normals = 0;

	if (m_texcoords)
		glDeleteBuffers(1, &m_texcoords);
	m_texcoords = 0;
}

void Renderer::Render()
{
	glBindVertexArray(m_vao);
	glUseProgram(m_shader.Program());

	glDrawElements(GL_TRIANGLES, m_model.Indices.size(), GL_UNSIGNED_SHORT, (void*)0);

	glBindVertexArray(0);
	glUseProgram(0);
}

bool Renderer::Success() const
{
	return m_success;
}
const std::string& Renderer::ErrorText() const
{
	return m_log;
}