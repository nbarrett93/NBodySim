#include "ShaderMgr.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <memory>

#include "ReadFile.h"

ShaderMgr::ShaderMgr()
{
}

ShaderMgr::~ShaderMgr()
{
}

bool ShaderMgr::LoadDir(const std::string &path)
{
	DWORD cur_dir_len = 0;

	if (!(cur_dir_len = GetCurrentDirectoryA(0, NULL)))
	{
		return false;
	}

	std::unique_ptr<char[]> cur_dir_buf = std::make_unique<char[]>(cur_dir_len);
	if (!GetCurrentDirectoryA(cur_dir_len, cur_dir_buf.get()))
	{
		return false;
	}

	SetCurrentDirectoryA(path.c_str());

	WIN32_FIND_DATAA results;
	HANDLE searcher = FindFirstFileExA(".\\*", FindExInfoBasic, &results, FindExSearchLimitToDirectories, NULL, NULL);

	if (INVALID_HANDLE_VALUE == searcher)
	{
		SetCurrentDirectoryA(cur_dir_buf.get());
		return false;
	}

	do
	{
		if (0 ==strcmp(results.cFileName, "."))
			continue;
		if (0 == strcmp(results.cFileName, ".."))
			continue;

		SetCurrentDirectoryA(results.cFileName);

		std::string prog_name(results.cFileName);

		std::vector<std::string> fragment_shaders;
		std::vector<std::string> vertex_shaders;

		// secondary search for files named *.vert | *.frag
		HANDLE inner_search = FindFirstFileExA(".\\*.frag", FindExInfoBasic, &results, FindExSearchNameMatch, NULL, NULL);

		if (INVALID_HANDLE_VALUE != inner_search)
		{
			do
			{
				DWORD path_len = GetFullPathNameA(results.cFileName, 0, NULL, NULL);
				std::vector<char> buf(path_len);
				GetFullPathNameA(results.cFileName, path_len, &buf[0], NULL);

				fragment_shaders.push_back(std::string(buf.begin(), buf.end()));

			} while (FindNextFileA(inner_search, &results));

			FindClose(inner_search);
		}

		inner_search = FindFirstFileExA(".\\*.vert", FindExInfoBasic, &results, FindExSearchNameMatch, NULL, NULL);

		if (INVALID_HANDLE_VALUE != inner_search)
		{
			do
			{
				DWORD path_len = GetFullPathNameA(results.cFileName, 0, NULL, NULL);
				std::vector<char> buf(path_len);
				GetFullPathNameA(results.cFileName, path_len, &buf[0], NULL);

				vertex_shaders.push_back(std::string(buf.begin(), buf.end()));

			} while (FindNextFileA(inner_search, &results));

			FindClose(inner_search);
		}

		ProgramFiles_ pf = std::make_pair(vertex_shaders, fragment_shaders);
		m_uncompiled.insert(std::make_pair(prog_name, pf));
		
		SetCurrentDirectoryA(".\\..");

	} while (FindNextFileA(searcher, &results));

	FindClose(searcher);

	SetCurrentDirectoryA(cur_dir_buf.get());

	return true;
}

GLint ShaderMgr::Get(const std::string &name)
{
	auto prog_result = m_programs.find(name);

	if (m_programs.end() != prog_result)
	{
		return prog_result->second;
	}

	auto source_result = m_uncompiled.find(name);
    if (m_uncompiled.end() == source_result)
    {
        m_success = false;
        m_log += "Program does not exist: " + name + "\n";
        return -1;
    }

	std::tuple<FileList_, FileList_> &sources = source_result->second;

	GLuint vert_shader, frag_shader;

	std::vector<std::string> vert_source_codes;

	bool succ = false;

	for (auto &s : std::get<0>(sources))
	{
		std::string s_ = ReadFile(s, succ);

		if (succ)
			vert_source_codes.push_back(s_);
        else
        {
            m_log += "Could not read source file: " + s + "\n";
            m_success = false;
            return -1;
        }
	}

	std::vector<std::string> frag_source_codes;

	for (auto &s : std::get<1>(sources))
	{
		std::string s_ = ReadFile(s, succ);

		if (succ)
			frag_source_codes.push_back(s_);
        else
        {
            m_log += "Could not read source file: " + s + "\n";
            m_success = false;
            return -1;
        }
	}

	std::vector<const char*> vert_source_pointers;
    vert_source_pointers.reserve(vert_source_codes.size());

	for (auto &s : vert_source_codes)
		vert_source_pointers.push_back(s.c_str());

	std::vector<const char*> frag_source_pointers;
    frag_source_pointers.reserve(frag_source_codes.size());

	for (auto &s : frag_source_codes)
		frag_source_pointers.push_back(s.c_str());

	vert_shader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vert_shader, vert_source_pointers.size(), &vert_source_pointers[0], NULL);
	glCompileShader(vert_shader);

    GLint gl_res;
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &gl_res);
    if (GL_FALSE == gl_res)
    {
        GLint maxLength = 0;
        glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(vert_shader, maxLength, &maxLength, &errorLog[0]);

        m_log += std::string(errorLog.begin(), errorLog.end()) + "\n";

        m_success = false;
        glDeleteShader(vert_shader);

        return -1;
    }

    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(frag_shader, frag_source_pointers.size(), &frag_source_pointers[0], NULL);
	glCompileShader(frag_shader);

    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &gl_res);
    if (GL_FALSE == gl_res)
    {
        GLint maxLength = 0;
        glGetShaderiv(frag_shader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(frag_shader, maxLength, &maxLength, &errorLog[0]);

        m_log += std::string(errorLog.begin(), errorLog.end()) + "\n";

        m_success = false;
        glDeleteShader(frag_shader);
        glDeleteShader(vert_shader);

        return -1;
    }

    GLint program = glCreateProgram();
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);

    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &gl_res);
    if (gl_res == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
        m_log += std::string(infoLog.begin(), infoLog.end()) + "\n";

        glDeleteProgram(program);

        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);

        m_success = false;

        return -1;
    }

    glDetachShader(program, vert_shader);
    glDetachShader(program, frag_shader);
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    m_success = true;

    m_programs.insert(std::make_pair(name, program));

    return program;
}

void ShaderMgr::Remove(const std::string &name)
{
    GLint x = Get(name);
    glDeleteProgram(x);
}

GLint ShaderMgr::Own(const std::string &name)
{
    GLint ret = Get(name);
    m_programs.erase(name);
    m_uncompiled.erase(name);

    return ret;
}

void ShaderMgr::Unload()
{

}

bool ShaderMgr::Success() const
{
	return m_success;
}

const std::string& ShaderMgr::ErrorLog() const
{
	return m_log;
}