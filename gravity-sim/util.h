#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <CL/opencl.h>

std::string read_file(const std::string &filename, bool &succ);

struct CL_Components
{
private:
	bool m_success;
	cl_uint m_cl_err;

public:
	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;

	CL_Components() : platform(0), device(0), context(0), queue(0), m_success(false), m_cl_err(0) { };
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
};

CL_Components get_CL_components(bool &succ);
bool finish_CL_components(CL_Components &comp, const cl_context_properties *properties);

#endif