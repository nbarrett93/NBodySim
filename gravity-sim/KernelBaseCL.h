#ifndef KERNEL_BASE_CL_H
#define KERNEL_BASE_CL_H

#include "SystemCL.h"

// TODO:
//	Remove Program from this class & put it into it's own class w/ proper ownership
class KernelBaseCL
{
protected:
	cl_program m_program;
	cl_kernel m_kernel;

	SystemCL m_system;
	std::string m_log;
	bool m_success;

public:
	KernelBaseCL(const SystemCL &system);
	KernelBaseCL(const KernelBaseCL &rhs);
	virtual ~KernelBaseCL();

	virtual void Load(const std::string &code, const std::string &kernel_name);
	virtual void Dispose();

	cl_kernel Kernel() const;
	const std::string& ErrorText() const;
	bool Success() const;
};

#endif