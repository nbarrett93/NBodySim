#ifndef SYSTEM_CL_H
#define SYSTEM_CL_H

#define NOMINMAX

#include <CL/opencl.h>
#include <string>
#include <Windows.h>

class SystemCL
{
private:
	cl_platform_id m_platform;
	cl_device_id m_device;
	cl_context m_context;
	cl_command_queue m_queue;

	std::string m_log;
	bool m_success;

public:
	SystemCL();
	SystemCL(const SystemCL &rhs);
	SystemCL(SystemCL &&rhs);
	~SystemCL();

	void Load(/*Input configuration of sorts. TODO.*/);
	void CreateContext(HGLRC glContext, HDC glDC);
	void Dispose();

	cl_platform_id Platform() const;
	cl_device_id Device() const;
	cl_context Context() const;
	cl_command_queue CommandQueue() const;

	bool Success() const;
	const std::string& ErrorLog() const;
};

#endif