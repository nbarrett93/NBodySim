#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <GL/glew.h>
#include <string>

struct Shader
{
private:
	bool m_error;

	Shader(const Shader &other) {};
public:
	GLuint Program;
	std::string ErrorText;

	bool Error() const {
		return m_error;
	};

	GLuint TakeProgram();
	~Shader();
	Shader(Shader &&rhs);
	Shader();

	void LoadProgram(const std::string &v_shader, const std::string &f_shader);
};

#endif