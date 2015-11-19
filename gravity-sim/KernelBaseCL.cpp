#include "KernelBaseCL.h"

KernelBaseCL::KernelBaseCL(const SystemCL &system) :
m_system(system),
m_success(false),
m_program(0),
m_kernel(0)
{
}

KernelBaseCL::KernelBaseCL(const KernelBaseCL &rhs) :
m_system(rhs.m_system),
m_success(rhs.m_success),
m_program(rhs.m_program),
m_kernel(rhs.m_kernel)
{
	// increase ref counts of CL objects
	clRetainKernel(m_kernel);
	clRetainProgram(m_program);
}

KernelBaseCL::~KernelBaseCL()
{
	Dispose();
}

void KernelBaseCL::Load(const std::string &code, const std::string &kernel_name)
{
	const char* code_ = code.c_str();
	const char* name = kernel_name.c_str();
	cl_int cl_err;

	cl_program prog = clCreateProgramWithSource(m_system.Context(), 1, &code_, NULL, &cl_err);
	if (cl_err != CL_SUCCESS)
	{
		m_success = false;
		m_log += "[FAIL] Unable to create program: EC = " + std::to_string(cl_err) + "\n";
		return;
	}

	cl_device_id dev = m_system.Device();
	cl_err = clBuildProgram(prog, 1, &dev, NULL, NULL, NULL);
	if (cl_err != CL_SUCCESS)
	{
		m_success = false;
		m_log += "[FAIL] Unable to build program: EC = " + std::to_string(cl_err) + "\n";

		clReleaseProgram(prog);

		return;
	}

	m_program = prog;

	cl_kernel kern = clCreateKernel(m_program, name, &cl_err);
	if (cl_err != CL_SUCCESS)
	{
		m_success = false;
		m_log += "[FAIL] Unable to build kernel: EC = " + std::to_string(cl_err) + "\n";

		clReleaseProgram(prog);

		return;
	}

	m_kernel = kern;
}

void KernelBaseCL::Dispose()
{
	if (m_kernel)
		clReleaseKernel(m_kernel);
	m_kernel = 0;
	if (m_program)
		clReleaseProgram(m_program);
	m_program = 0;
}

cl_kernel KernelBaseCL::Kernel() const
{
	return m_kernel;
}
const std::string& KernelBaseCL::ErrorText() const
{
	return m_log;
}
bool KernelBaseCL::Success() const
{
	return m_success;
}