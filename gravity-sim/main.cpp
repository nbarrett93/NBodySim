#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <iostream>
#include <vector>
#include <cstdint>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <CL/opencl.h>
#include <Windows.h>

#include "util.h"
#include "particle.h"

static void error_callback(int error, const char* description)
{
	std::cerr << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main()
{
	bool succ = false;
	CL_Components cl_objs = get_CL_components(succ);

	if (!succ)
		return -1;

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
	
	cl_context_properties properties[] = 
	{
		CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
		CL_CONTEXT_PLATFORM, (cl_context_properties)cl_objs.platform,
		0
	};

	if (!finish_CL_components(cl_objs, properties))
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	/*
	Initialize OpenCL & OpenGL memory structures here
	/****************************************************/
	ParticleSystem system(cl_objs);

	glfwSwapInterval(1);

	glfwSetKeyCallback(window, key_callback);

	glClearColor(0, 1.0f, 0, 0);

	while (!glfwWindowShouldClose(window))
	{
		/*
		Do OpenCL stuff here
		/*************************************************/



		/*
		Do OpenGL stuff here
		/*************************************************/


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		glFinish();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
