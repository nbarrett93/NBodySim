#include "ShaderLoader.h"

#include <vector>

void Shader::LoadProgram(const std::string &v_shader, const std::string &f_shader)
{
	m_error = false;

	GLuint program, vert, frag;

	vert = glCreateShader(GL_VERTEX_SHADER);

	const char* src = v_shader.c_str();

	glShaderSource(vert, 1, &src, NULL);
	glCompileShader(vert);

	GLint succ;
	glGetShaderiv(vert, GL_COMPILE_STATUS, &succ);
	if (succ == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vert, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(vert, maxLength, &maxLength, &errorLog[0]);

		ErrorText = std::string(errorLog.begin(), errorLog.end());

		m_error = true;
		glDeleteShader(vert);

		return;
	}

	frag = glCreateShader(GL_FRAGMENT_SHADER);
	src = f_shader.c_str();

	glShaderSource(frag, 1, &src, NULL);
	glCompileShader(frag);

	glGetShaderiv(frag, GL_COMPILE_STATUS, &succ);
	if (succ == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vert, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(vert, maxLength, &maxLength, &errorLog[0]);

		ErrorText = std::string(errorLog.begin(), errorLog.end());

		m_error = true;
		glDeleteShader(frag);
		glDeleteShader(vert);

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

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		ErrorText = std::string(infoLog.begin(), infoLog.end());

		glDeleteProgram(program);

		glDeleteShader(vert);
		glDeleteShader(frag);

		m_error = true;

		return;
	}

	glDetachShader(program, vert);
	glDeleteShader(vert);

	glDetachShader(program, frag);
	glDeleteShader(frag);

	Program = program;
}

Shader::~Shader()
{
	glDeleteProgram(Program);
}

GLuint Shader::TakeProgram()
{
	GLuint p = Program;
	Program = 0;
	return p;
}

Shader::Shader(Shader &&rhs) :
	Program(rhs.Program),
	m_error(rhs.m_error),
	ErrorText(std::move(rhs.ErrorText))
{
	rhs.Program = 0;
}

Shader::Shader() :
	m_error(false)
{}