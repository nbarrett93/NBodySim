#include "SystemCL.h"


SystemCL::SystemCL() :
m_context(0),
m_device(0),
m_platform(0),
m_queue(0)
{
}

SystemCL::SystemCL(const SystemCL& rhs) :
m_context(rhs.m_context),
m_device(rhs.m_device),
m_platform(rhs.m_platform),
m_queue(rhs.m_queue),
m_success(rhs.m_success),
m_log(rhs.m_log)
{
	// reference count of all CL objects needs to increase
	clRetainContext(m_context);
	clRetainCommandQueue(m_queue);
}

SystemCL::SystemCL(SystemCL &&rhs) :
m_context(rhs.m_context),
m_device(rhs.m_device),
m_platform(rhs.m_platform),
m_queue(rhs.m_queue),
m_log(std::move(rhs.m_log)),
m_success(rhs.m_success)
{
	// reference counts stay the same since we're taking them
	rhs.m_context = 0;
	rhs.m_device = 0;
	rhs.m_platform = 0;
	rhs.m_queue = 0;
	rhs.m_success = false;
}

SystemCL::~SystemCL()
{
	if (m_queue)
		clReleaseCommandQueue(m_queue);
	if (m_context)
		clReleaseContext(m_context);
}

cl_platform_id SystemCL::Platform() const
{
	return m_platform;
}

cl_device_id SystemCL::Device() const
{
	return m_device;
}

cl_context SystemCL::Context() const
{
	return m_context;
}

cl_command_queue SystemCL::CommandQueue() const
{
	return m_queue;
}

const std::string& SystemCL::ErrorLog() const
{
	return m_log;
}

bool SystemCL::Success() const
{
	return m_success;
}

void SystemCL::Load()
{
	cl_platform_id *platform_ids;
	cl_uint num_platforms;

	cl_int cl_ret;

	m_success = false;

	// figure out how many platforms there are
	cl_ret = clGetPlatformIDs(0, NULL, &num_platforms);
	if (cl_ret != CL_SUCCESS)
	{
		m_log += "[FAIL] Error getting number of platforms: EC = ";
		m_log += std::to_string(cl_ret);
		m_log += "\n";

		return;
	}

	// ensure we have at least 1 platform
	if (num_platforms == 0)
	{
		m_log += "[FAIL] No platforms available\n";

		return;
	}

	platform_ids = new cl_platform_id[num_platforms];

	// read out all platform IDs
	cl_ret = clGetPlatformIDs(num_platforms, platform_ids, NULL);
	if (cl_ret != CL_SUCCESS)
	{
		m_log += "[FAIL] Error retrieving platforms: EC = ";
		m_log += std::to_string(cl_ret);
		m_log += "\n";

		if (platform_ids != NULL)
			delete[] platform_ids;

		return;
	}

	m_log += "[INFO] " + std::to_string(num_platforms) + " platform(s) found\n";

	for (cl_uint i = 0; i < num_platforms; ++i)
	{
		cl_uint num_devices;
		// count # of devices which are GPU
		cl_ret = clGetDeviceIDs(platform_ids[i], CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices);
		if (cl_ret == CL_DEVICE_NOT_FOUND)
		{
			continue;
		}

		if (num_devices != 0)
		{
			// since we are guaranteed some devices, read their IDs out
			cl_device_id *device_ids = new cl_device_id[num_devices];

			if (clGetDeviceIDs(platform_ids[i], CL_DEVICE_TYPE_GPU, num_devices, device_ids, NULL) != CL_SUCCESS)
			{
				if (device_ids != NULL)
					delete[] device_ids;

				continue;
			}

			// default device (hopefully a good one!)
			// TODO: should probably dump all devices to log
			m_device = device_ids[0];
			m_platform = platform_ids[i];

			if (device_ids != NULL)
				delete[] device_ids;

			if (platform_ids != NULL)
				delete[] platform_ids;

			// stop scanning platforms; we found a GPU
			m_success = true;
			m_log += "[INFO] Platform and device found\n";
			return;
		}
	}

	if (platform_ids != NULL)
		delete[] platform_ids;

	m_log += "[FAIL] Unable to find a GPU\n";
	return;
}

void SystemCL::CreateContext(HGLRC glContext, HDC glDC)
{
	m_success = false;
	cl_int cl_err;

	cl_context_properties properties[] =
	{
		CL_GL_CONTEXT_KHR, (cl_context_properties)glContext,
		CL_WGL_HDC_KHR, (cl_context_properties)glDC,
		CL_CONTEXT_PLATFORM, (cl_context_properties)m_platform,
		0
	};

	cl_context cl_ctx = clCreateContext(properties, 1, &m_device, NULL, NULL, &cl_err);

	if (cl_err == CL_SUCCESS)
	{
		cl_command_queue queue_ = clCreateCommandQueue(cl_ctx, m_device, NULL, &cl_err);

		if (cl_err != CL_SUCCESS)
		{
			m_log += "[FAIL] Error creating command queue: EC = " + std::to_string(cl_err) + "\n";
			clReleaseContext(cl_ctx);
			return;
		}

		m_queue = queue_;
		m_context = cl_ctx;
		m_success = true;
		return;
	}

	m_log += "[FAIL] Error creating context: EC = " + std::to_string(cl_err) + "\n";
}

void SystemCL::Dispose()
{
	if (m_queue)
		clReleaseCommandQueue(m_queue);
	m_queue = 0;

	if (m_context)
		clReleaseContext(m_context);
	m_context = 0;

	m_device = 0;
}