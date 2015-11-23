#include "ShaderBase.h"

#include <vector>

ShaderBase::ShaderBase() : m_program(0), m_succeeded(false), m_log("")
{
}

void ShaderBase::Load(ShaderMgr &sm, const std::string name)
{
    m_program = sm.Get(name);
    if (!sm.Success())
    {
        m_succeeded = false;
        m_log += "Error in ShaderBase: " + sm.ErrorLog() + "\n";
        m_program = -1;
    }
    m_succeeded = true;
}

void ShaderBase::Dispose()
{
	m_program = 0;
}

GLuint ShaderBase::Program() const
{
	return m_program;
}

bool ShaderBase::Success() const
{
	return m_succeeded;
}

const std::string& ShaderBase::ErrorText() const
{
	return m_log;
}