#ifndef BOUNCE_KERNEL_H
#define BOUNCE_KERNEL_H

#include "KernelBaseCL.h"
#include <string>

class BounceKernel : public KernelBaseCL
{
private:
	cl_mem m_position;
	size_t m_num_objects;

	static const std::string ProgramFileName;
	static const std::string KernelName;

	BounceKernel(const BounceKernel&);

public:
	BounceKernel(const SystemCL &sys);
	~BounceKernel();

	void Load(cl_GLuint pos_buffer, size_t num_objects);
	void Dispose();

	void Run(float time);
};

#endif