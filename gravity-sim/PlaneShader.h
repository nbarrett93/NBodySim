#ifndef PLANE_SHADER_H
#define PLANE_SHADER_H

#include "ShaderBase.h"
#include <GL/glew.h>

class PlaneShader : public ShaderBase
{
private:
	GLint m_pos_loc;
	GLint m_uv_loc;
	GLint m_samp_loc;

	const static std::string PositionName;
	const static std::string TextCoordsName;
	const static std::string SamplerName;

public:
	PlaneShader();

    void Load(ShaderMgr &sm);

	GLint PositionLoc() const;
	GLint TexCoordsLoc() const;
	GLint SamplerULoc() const;
};

#endif