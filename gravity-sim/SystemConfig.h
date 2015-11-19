#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include "CamSettings.h"
#include <cstdint>
#include <string>

struct ParticleSystemConfig
{
	CameraSettings CamSettings;
	uint32_t NumParticles;
	glm::vec4 *ParticlePositions;
	glm::vec4 *ParticleVelocities;
	float *ParticleMasses;
};

#endif