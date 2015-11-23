#include "PlanesRenderer.h"

const std::string PlanesRenderer::TexturePath = ".\\resources\\textures\\coord-grid.dds";

PlanesRenderer::PlanesRenderer() :
m_texture(0)
{
	m_model.Vertices.insert(m_model.Vertices.begin(), {
		glm::vec4(128.0f, 0.0f, 128.0f, 1.0f),
		glm::vec4(128.0f, 0.0f, -128.0f, 1.0f),
		glm::vec4(-128.0f, 0.0f, 128.0f, 1.0f),
		glm::vec4(-128.0f, 0.0f, -128.0f, 1.0f)
	});

	m_model.TexCoords.insert(m_model.TexCoords.begin(), {
		glm::vec2(256.0f, 256.0f),
		glm::vec2(256.0f, 0.0f),
		glm::vec2(0.0f, 256.0f),
		glm::vec2(0.0f, 0.0f)
	});

	m_model.Indices.insert(m_model.Indices.begin(), { 
		0, 1, 2, 2, 1, 3 
	});

	// no normals
}

PlanesRenderer::~PlanesRenderer()
{
	Dispose();
}

void PlanesRenderer::Load()
{

}

void PlanesRenderer::Dispose()
{
	if (m_texture)
		glDeleteTextures(1, &m_texture);
	m_texture = 0;
}

void PlanesRenderer::Render()
{

}