#include "util.h"

#include <fstream>
#include <cstdint>

std::string read_file(const std::string &filename, bool &succ)
{
	succ = true;

	std::ifstream file_stream(filename, std::ios_base::in);

	if (file_stream.is_open())
		return std::string((std::istreambuf_iterator<char>(file_stream)), 
							std::istreambuf_iterator<char>());

	succ = false;
	return std::string();
}

CL_Components& CL_Components::init()
{
	m_success = false;

	cl_platform_id *platform_ids;
	cl_uint num_platforms;

	// figure out how many platforms there are
	if (clGetPlatformIDs(0, NULL, &num_platforms) != CL_SUCCESS)
	{
		return *this;
	}

	// ensure we have at least 1 platform
	if (num_platforms == 0)
	{
		return *this;
	}

	platform_ids = new cl_platform_id[num_platforms];

	// read out all platform IDs
	if (clGetPlatformIDs(num_platforms, platform_ids, NULL) != CL_SUCCESS)
	{
		if (platform_ids != NULL) 
			delete[] platform_ids;
		return *this;
	}

	for (cl_uint i = 0; i < num_platforms; ++i)
	{
		// TODO: dump platform info to log

		cl_uint num_devices;
		// count # of devices which are GPU
		if (clGetDeviceIDs(platform_ids[i], CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices) != CL_SUCCESS)
		{
			if (platform_ids != NULL)
				delete[] platform_ids;
			return *this;
		}

		if (num_devices != 0)
		{
			// since we are guaranteed some devices, read their IDs out
			cl_device_id *device_ids = new cl_device_id[num_devices];

			if (clGetDeviceIDs(platform_ids[i], CL_DEVICE_TYPE_GPU, num_devices, device_ids, NULL) != CL_SUCCESS)
			{
				if (device_ids != NULL) 
					delete[] device_ids;
				if (platform_ids != NULL) 
					delete[] platform_ids;
				return *this;
			}

			// default device (hopefully a good one!)
			// TODO: should probably dump all devices to log
			device = device_ids[0];
			platform = platform_ids[i];

			// TODO: should dump some info about the device to log

			if (device_ids != NULL) 
				delete[] device_ids;

			// stop scanning platforms; we found a GPU
			m_success = true;
			break;
		}
	}

	if (platform_ids != NULL)
		delete[] platform_ids;

	return *this;
}

CL_Components& CL_Components::setCtx(HGLRC ctx, HDC dc)
{
	m_success = false;

	cl_context_properties properties[] =
	{
		CL_GL_CONTEXT_KHR, (cl_context_properties)ctx,
		CL_WGL_HDC_KHR, (cl_context_properties)dc,
		CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
		0
	};

	cl_context cl_ctx = clCreateContext(properties, 1, &device, NULL, NULL, &m_cl_err);

	if (m_cl_err == CL_SUCCESS)
	{
		cl_command_queue queue_ = clCreateCommandQueue(cl_ctx, device, NULL, &m_cl_err);

		if (m_cl_err != CL_SUCCESS)
		{
			m_success = false;
			clReleaseContext(cl_ctx);
			return *this;
		}

		queue = queue_;
		context = cl_ctx;
		m_success = true;
	}

	return *this;
}

/*CL_Components::CL_Components(CL_Components &&rhs) :
	m_success(rhs.m_success),
	m_cl_err(rhs.m_cl_err),
	platform(rhs.platform),
	device(rhs.device),
	context(rhs.context),
	queue(rhs.queue)
{
	rhs.platform = 0;
	rhs.device = 0;
	rhs.context = 0;
	rhs.queue = 0;
}*/

void CL_Components::dispose()
{
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
}