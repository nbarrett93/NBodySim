#include "ParticleShader.h"

#include "ReadFile.h"

const std::string ParticleShader::LightPosName = "light_pos_world";
const std::string ParticleShader::PositionName = "position";
const std::string ParticleShader::NormalName = "normal";
const std::string ParticleShader::ViewName = "v_matrix";
const std::string ParticleShader::MVPName = "mvp_matrix";
const std::string ParticleShader::ModelPosName = "offset";

const std::string ParticleShader::VertexShader = ".\\resources\\shaders\\shader.vert";
const std::string ParticleShader::FragmentShader = ".\\resources\\shaders\\shader.frag";

ParticleShader::ParticleShader() :
m_lightpos_loc(-1),
m_modelpos_loc(-1),
m_mvp_loc(-1),
m_view_loc(-1),
m_position_loc(-1),
m_normal_loc(-1)
{
}

void ParticleShader::Load()
{
	bool FileReadSucc = false;

	std::string vert_s = ReadFile(VertexShader, FileReadSucc);
	if (!FileReadSucc)
	{
		m_succeeded = false;
		m_log += "Error reading vertex shader\n";

		return;
	}

	std::string frag_s = ReadFile(FragmentShader, FileReadSucc);
	if (!FileReadSucc)
	{
		m_succeeded = false;
		m_log += "Error reading fragment shader\n";

		return;
	}

	ShaderBase::Load(vert_s, frag_s);

	if (!Success())
		return;

	m_lightpos_loc = glGetUniformLocation(Program(), LightPosName.c_str());
	m_mvp_loc = glGetUniformLocation(Program(), MVPName.c_str());
	m_view_loc = glGetUniformLocation(Program(), ViewName.c_str());

	m_modelpos_loc = glGetAttribLocation(Program(), ModelPosName.c_str());
	m_position_loc = glGetAttribLocation(Program(), PositionName.c_str());
	m_normal_loc = glGetAttribLocation(Program(), NormalName.c_str());
}

GLint ParticleShader::PositionLoc() const
{
	return m_position_loc;
}

GLint ParticleShader::ModelPosLoc() const
{
	return m_modelpos_loc;
}

GLint ParticleShader::NormalLoc() const
{
	return m_normal_loc;
}

GLint ParticleShader::MVPULoc() const
{
	return m_mvp_loc;
}

GLint ParticleShader::ViewULoc() const
{
	return m_view_loc;
}

GLint ParticleShader::LightPosULoc() const
{
	return m_lightpos_loc;
}