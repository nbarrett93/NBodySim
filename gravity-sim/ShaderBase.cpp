#include "ShaderBase.h"

#include <vector>

ShaderBase::ShaderBase() : m_program(0), m_succeeded(false), m_log("")
{
}

void ShaderBase::Load(const std::string &vert_s, const std::string &frag_s)
{
	GLuint program, vert, frag;

	vert = glCreateShader(GL_VERTEX_SHADER);

	const char* src = vert_s.c_str();

	glShaderSource(vert, 1, &src, NULL);
	glCompileShader(vert);

	GLint succ;
	glGetShaderiv(vert, GL_COMPILE_STATUS, &succ);
	if (succ == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vert, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(vert, maxLength, &maxLength, &errorLog[0]);

		m_log += std::string(errorLog.begin(), errorLog.end()) + "\n";

		m_succeeded = false;
		glDeleteShader(vert);

		return;
	}

	frag = glCreateShader(GL_FRAGMENT_SHADER);
	src = frag_s.c_str();

	glShaderSource(frag, 1, &src, NULL);
	glCompileShader(frag);

	glGetShaderiv(frag, GL_COMPILE_STATUS, &succ);
	if (succ == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vert, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(vert, maxLength, &maxLength, &errorLog[0]);

		m_log += std::string(errorLog.begin(), errorLog.end()) + "\n";

		glDeleteShader(frag);
		glDeleteShader(vert);

		m_succeeded = false;

		return;
	}

	program = glCreateProgram();

	glAttachShader(program, vert);
	glAttachShader(program, frag);

	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &succ);
	if (succ == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		m_log += std::string(infoLog.begin(), infoLog.end()) + "\n";

		glDeleteProgram(program);

		glDeleteShader(vert);
		glDeleteShader(frag);

		m_succeeded = false;

		return;
	}

	glDetachShader(program, vert);
	glDeleteShader(vert);

	glDetachShader(program, frag);
	glDeleteShader(frag);

	m_succeeded = true;

	m_program = program;
}

void ShaderBase::Dispose()
{
	if (m_program)
		glDeleteProgram(m_program);
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