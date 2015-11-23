#ifndef INST_SPHERE_RENDERER_H
#define INST_SPHERE_RENDERER_H

#include "Renderer.h"
#include "ParticleShader.h"
#include "camera.h"
#include <GL/glew.h>
#include <vector>

class InstSphereRenderer : public Renderer
{
private:
	GLuint m_instance;
	size_t m_num_particles;

	InstSphereRenderer(const InstSphereRenderer&)
	{};

	static const std::string ModelPath;

public:
	InstSphereRenderer();
	~InstSphereRenderer();

	void Load(const glm::vec4 *positions, size_t num_particles, ShaderMgr &sm);

	// need to abstract lighting out into a class
	void Render(const Camera &cam, const glm::vec4 &light_pos);
	void Dispose();

	GLuint BufferLoc() const;
};

#endif