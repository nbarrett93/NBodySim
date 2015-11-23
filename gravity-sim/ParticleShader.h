#ifndef PARTICLE_SHADER_H
#define PARTICLE_SHADER_H

#include "ShaderBase.h"
#include <GL/glew.h>

class ParticleShader : public ShaderBase
{
private:
	GLint m_position_loc;
	GLint m_modelpos_loc;
	GLint m_normal_loc;

	GLint m_mvp_loc;
	GLint m_view_loc;
	GLint m_lightpos_loc;

	static const std::string LightPosName;
	static const std::string PositionName;
	static const std::string NormalName;
	static const std::string ViewName;
	static const std::string MVPName;
	static const std::string ModelPosName;

public:
	ParticleShader();

	void Load(ShaderMgr &sm);

	GLint PositionLoc() const;
	GLint ModelPosLoc() const;
	GLint NormalLoc() const;

	GLint MVPULoc() const;
	GLint ViewULoc() const;
	GLint LightPosULoc() const;
};

#endif