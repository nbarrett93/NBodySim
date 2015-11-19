#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include "CamSettings.h"
#include <cstdint>
#include <string>

struct ParticleSystemConfig
{
	CameraSettings CamSettings;
	uint32_t NumParticles;
	float *ParticlePositions;
	float *ParticleVelocities;
	float *ParticleMasses;
	std::string KernelSource;
	std::string ModelObjContents;
};

#endif