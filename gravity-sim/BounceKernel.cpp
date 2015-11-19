#include "BounceKernel.h"

#include "ReadFile.h"

const std::string BounceKernel::KernelName = "calc_next";
const std::string BounceKernel::ProgramFileName = ".\\resources\\kernels\\bounce_kernel.cl";

BounceKernel::BounceKernel(const SystemCL &system) :
KernelBaseCL(system),
m_position(0)
{
}

BounceKernel::BounceKernel(const BounceKernel &other) : 
KernelBaseCL(other),
m_position(other.m_position)
{
	if (m_position)
		clRetainMemObject(m_position);
};

BounceKernel::~BounceKernel()
{
	Dispose();
}

void BounceKernel::Load(cl_GLuint pos_buffer, size_t num_objects)
{
	m_num_objects = num_objects;

	std::string program_code = ReadFile(ProgramFileName, m_success);
	if (!m_success)
	{
		m_log += "[FAIL] Unable to read program file: " + ProgramFileName + "\n";
		return;
	}

	KernelBaseCL::Load(program_code, KernelName);

	if (!m_success)
		return;

	cl_int cl_err;
	cl_mem buf = clCreateFromGLBuffer(m_system.Context(), CL_MEM_READ_WRITE, pos_buffer, &cl_err);
	if (cl_err != CL_SUCCESS)
	{
		m_success = false;
		m_log += "[FAIL] Unable to create position buffer: EC = " + std::to_string(cl_err) + "\n";
		return;
	}

	m_success = true;
}

void BounceKernel::Dispose()
{
	if (m_position)
		clReleaseMemObject(m_position);
	m_position = 0;

	KernelBaseCL::Dispose();
}

void BounceKernel::Run(float time)
{
	m_success = true;

	cl_int cl_err;

	cl_err = clEnqueueAcquireGLObjects(m_system.CommandQueue(), 1, &m_position, 0, NULL, NULL);
	if (cl_err != CL_SUCCESS)
	{
		m_success = false;
		m_log += "[FAIL] Error acquiring GLObject Position: EC = " + std::to_string(cl_err) + "\n";
		return;
	}

	cl_err = clSetKernelArg(m_kernel, 0, sizeof(cl_mem), &m_position);
	if (cl_err != CL_SUCCESS)
	{
		m_log += "[FAIL] Error setting kernel argument Position: EC = " + std::to_string(cl_err) + "\n";
		m_success = false;
	}
	else
	{
		//cl_ret = clSetKernelArg(m_cl_kernel, 1, sizeof(cl_mem), &m_cl_time);
		cl_err = clSetKernelArg(m_kernel, 1, sizeof(float), &time);
		if (cl_err != CL_SUCCESS)
		{
			m_log += "[FAIL] Error setting kernel argument Time: EC = " + std::to_string(cl_err) + "\n";
			m_success = false;
		}
		else
		{
			cl_err = clEnqueueNDRangeKernel(m_system.CommandQueue(), m_kernel, 1, NULL, &m_num_objects, NULL, 0, NULL, NULL);
			if (cl_err != CL_SUCCESS)
			{
				m_log += "[FAIL] Error executing kernel: EC = " + std::to_string(cl_err) + "\n";
				m_success = false;
			}
		}
	}

	cl_err = clEnqueueReleaseGLObjects(m_system.CommandQueue(), 1, &m_position, 0, NULL, NULL);
	if (cl_err != CL_SUCCESS)
	{
		m_success = false;
		m_log += "[FAIL] Error releasing GLObject Position: EC = " + std::to_string(cl_err) + "\n";
	}

	clFinish(m_system.CommandQueue());
}