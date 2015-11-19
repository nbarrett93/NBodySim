#ifndef SHADER_BASE_H
#define SHADER_BASE_H

#include <string>
#include <GL/glew.h>

class ShaderBase
{
protected:
	GLuint m_program;
	bool m_succeeded;
	std::string m_log;

public:
	ShaderBase();
	virtual void Load(const std::string &vert, const std::string &frag);
	virtual void Dispose();

	GLuint Program() const;
	const std::string& ErrorText() const;
	bool Success() const;
};

#endif