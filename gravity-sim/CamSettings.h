#ifndef CAM_SETTINGS_H
#define CAM_SETTINGS_H

struct GLFWwindow;

struct CameraSettings
{
	float CamVelocity;
	float CamSensitivity;
	float FoV;
	float AspectRatio;
	GLFWwindow *Window;
};

#endif