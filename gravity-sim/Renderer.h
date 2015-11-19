#ifndef RENDERER_H
#define RENDERER_H

// has a shader & a model
// virtual render() func

#include <GL/glew.h>

#include "ObjLoader.hpp"
#include "ShaderBase.h"

class Renderer
{
protected:
	Model<3> m_model;
	ShaderBase m_shader;

	GLuint m_vao;
	GLuint m_vertices;
	GLuint m_normals;
	GLuint m_texcoords;
	GLuint m_indices;

	std::string m_log;
	bool m_success;

	Renderer(const Renderer&)
	{};

public:
	Renderer();
	virtual ~Renderer();

	void Render();
	void Load(Model<3> &model, ShaderBase &shader);

	virtual void Dispose();

	bool Success() const;
	const std::string& ErrorText() const;
};

#endif