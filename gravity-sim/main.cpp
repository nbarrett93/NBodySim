#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <iostream>
#include <vector>
#include <cstdint>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <CL/opencl.h>
#include <Windows.h>
#include <glm/common.hpp>

#include "util.h"
#include "particle.h"

static void error_callback(int error, const char* description)
{
	// TODO: make this go to log
	std::cerr << description << std::endl;
}

static void run_main_loop(CL_Components &&cl_state, GLFWwindow *window);

int main()
{
	// TODO: instantiate logger
	// TODO: instantiate config reader

	CL_Components cl_state;

	if (!cl_state.init().success())
	{
		// TODO: log failure
		return -1;
	}

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	// 4x AA
	glfwWindowHint(GLFW_SAMPLES, 4);
	// OpenGL 4.0
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(640, 480, "Gravity Simulator", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!cl_state.setCtx(wglGetCurrentContext(), wglGetCurrentDC()).success())
	{
		// TODO: log failure
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		// TODO: log failure
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	run_main_loop(std::move(cl_state), window);

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// TODO: send this info to the particle system
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

static void run_main_loop(CL_Components &&cl_state, GLFWwindow *window)
{
	// TODO: take these from config
	const float FieldOfView = 3.14159265359f / 3.0f,
		AspectRatio = 4.0f / 3.0f;

	ParticleSystem system(
		cl_state,
		{ FieldOfView, AspectRatio });

	glfwSwapInterval(1);

	glfwSetKeyCallback(window, key_callback);

	glClearColor(0, 0, 0, 0);

	float dt = 0.033f;
	bool first = true;
	glfwSetTime(0.0);

	while (!glfwWindowShouldClose(window))
	{
		dt = first ? dt : glfwGetTime();
		first = false;
		glfwSetTime(0.0);

		system.delta(dt);
		/*
		Removed for dev of GL system since CL depends upon it
		*/
		//system.run_CL();

		system.draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}