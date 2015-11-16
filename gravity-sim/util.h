#ifndef UTIL_H
#define UTIL_H

#define NOMINMAX

#include <string>
#include <CL/opencl.h>
#include <Windows.h>

std::string read_file(const std::string &filename, bool &succ);

// TODO: make this take in a configuration w/ platform, device, etc.
struct CL_Components
{
private:
	bool m_success;
	cl_int m_cl_err;

	CL_Components(CL_Components &rhs) {};

public:
	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;

	CL_Components() : platform(0), device(0), context(0), queue(0), m_success(false), m_cl_err(0) { };
	CL_Components(CL_Components &&rhs);
	~CL_Components()
	{
		if (queue)
			clReleaseCommandQueue(queue);
		if (context)
			clReleaseContext(context);
		if (device)
			clReleaseDevice(device);
	};

	bool success() const { return m_success; };
	CL_Components& init();
	CL_Components& setCtx(HGLRC ctx, HDC dc);
};

#endif