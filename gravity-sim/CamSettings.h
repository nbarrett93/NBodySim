#ifndef CAM_SETTINGS_H
#define CAM_SETTINGS_H

struct GLFWwindow;

struct CameraSettings
{
	double CamVelocity;
	double CamSensitivity;
	float FoV;
	float AspectRatio;
	GLFWwindow *Window;
};

#endif