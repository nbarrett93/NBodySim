#ifndef PLANES_RENDERER_H
#define PLANES_RENDERER_H

#include "Renderer.h"

class PlanesRenderer : public Renderer
{
private:
	GLuint m_texture;

	const static std::string TexturePath;

public:
	PlanesRenderer();
	~PlanesRenderer();

	void Render();
	void Load();

	void Dispose();
};

#endif