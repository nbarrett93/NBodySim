#ifndef SHADER_MGR_H
#define SHADER_MGR_H

#include <map>
#include <vector>
#include <utility>
#include <GL/glew.h>

class ShaderMgr
{
private:
	typedef std::vector<std::string> FileList_;
	typedef std::tuple<FileList_, FileList_> ProgramFiles_;
	typedef std::tuple<std::string, ProgramFiles_> Program_;

	std::map<std::string, GLint> m_programs;
	std::map<std::string, ProgramFiles_> m_uncompiled;

	std::string m_log;
	bool m_success;

public:
	ShaderMgr();
	~ShaderMgr();

	ShaderMgr(const ShaderMgr &other) = delete;
	ShaderMgr& operator=(const ShaderMgr &other) = delete;

	bool LoadDir(const std::string &path);

	GLint Get(const std::string &name);
	void Remove(const std::string &name);
    GLint Own(const std::string &name);

	void Unload();

	bool Success() const;
	const std::string& ErrorLog() const;
};

#endif