#include "PlaneShader.h"

#include "ReadFile.h"

const std::string PlaneShader::PositionName = "";
const std::string PlaneShader::TextCoordsName = "";
const std::string PlaneShader::SamplerName = "";

PlaneShader::PlaneShader() :
m_pos_loc(-1),
m_uv_loc(-1),
m_samp_loc(-1)
{
	
}

void PlaneShader::Load(ShaderMgr &sm)
{
    ShaderBase::Load(sm, "coord-plane");
    if (!m_succeeded)
    {
        return;
    }

	m_pos_loc = glGetAttribLocation(m_program, PositionName.c_str());
	m_uv_loc = glGetAttribLocation(m_program, TextCoordsName.c_str());

	m_samp_loc = glGetUniformLocation(m_program, SamplerName.c_str());
}

GLint PlaneShader::PositionLoc() const
{
	return m_pos_loc;
}
GLint PlaneShader::TexCoordsLoc() const
{
	return m_uv_loc;
}
GLint PlaneShader::SamplerULoc() const
{
	return m_samp_loc;
}