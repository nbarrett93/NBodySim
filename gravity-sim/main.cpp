//#pragma comment(linker, "/SUBSYSTEM:windows")
#pragma comment(linker, "/ENTRY:mainCRTStartup")

#include <iostream>
#include <vector>
#include <cstdint>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <CL/opencl.h>
#include <Windows.h>
#include <glm/common.hpp>

#include "util.h"
#include "particle.h"

#include "ObjLoader.hpp"

static void error_callback(int error, const char* description)
{
	// TODO: make this go to log
	std::cerr << description << std::endl;
}

static void run_main_loop(CL_Components &&cl_state, GLFWwindow *window);

static GLfloat dummy_positions[3 * 4] =
{
	1.0, 0.0, -1.0, 0.0,
	0.0, 0.0, 0.0, 0.0,
	-1.0, 0.0, -1.0, 0.0
};

static GLfloat dummy_masses[3] =
{
	1.0, 2.0, 4.0
};

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

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Gravity Simulator", NULL, NULL);
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

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	run_main_loop(std::move(cl_state), window);

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// TODO: send this info to the particle system
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}

	ParticleSystem *system = (ParticleSystem*)glfwGetWindowUserPointer(window);
	if (system == NULL)
		return;

	system->HandleKey(key, action);
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	// normalize the coordinates
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	xpos /= (double)width;
	ypos /= (double)height;

	float x, y;
	x = -1.0 + 2.0 * xpos;
	y = 1.0 - 2.0 * ypos;

	ParticleSystem *system = (ParticleSystem*)glfwGetWindowUserPointer(window);
	if (system == NULL)
		return;

	system->HandleMouse(x, y);
}

static void button_callback(GLFWwindow* window, int button, int action, int mods)
{
	ParticleSystem *system = (ParticleSystem*)glfwGetWindowUserPointer(window);
	if (system == NULL)
		return;

	system->HandleButton(button, action);
}

static void run_main_loop(CL_Components &&cl_state, GLFWwindow *window)
{
	// TODO: take these from config
	const float FieldOfView = 3.14159265359f / 3.0f,
		AspectRatio = 1280.0f / 720.0f;
	bool succ = false;

	ParticleSystemConfig cfg;
	cfg.CamSensitivity = 0.5;
	cfg.CamVelocity = 3;
	cfg.Window = window;
	cfg.FoV = FieldOfView;
	cfg.AspectRatio = AspectRatio;
	cfg.ModelObjContents = read_file(".\\resources\\models\\sphere.obj", succ);
	if (!succ)
	{
		// TODO: log error in reading sphere model
		return;
	}
	cfg.VertShader = read_file(".\\resources\\shaders\\test_shd.vert", succ);
	if (!succ)
	{
		// TODO: log error in reading vertex shader
		return;
	}
	cfg.FragShader = read_file(".\\resources\\shaders\\test_shd.frag", succ);
	if (!succ)
	{
		// TODO: log error in reading fragment shader
		return;
	}

	// dummy values for testing
	cfg.NumParticles = 3;
	cfg.ParticlePositions = dummy_positions;
	cfg.ParticleMasses = dummy_masses;

	ParticleSystem system(
		std::move(cl_state),
		cfg
	);

	system.Init();

	if (!system.good())
	{
		std::cout << "Error initializing system." << std::endl;
		std::cout << system.err_log() << std::endl;
		std::cin.get();
		return;
	}

	glfwSwapInterval(1);

	glfwSetWindowUserPointer(window, (void*)&system);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, button_callback);

	glClearColor(0.0, 0.0, 0.0, 0);

	float dt = 0.033f;
	bool first = true;
	glfwSetTime(0.0);

	while (!glfwWindowShouldClose(window) && system.good())
	{
		dt = first ? dt : glfwGetTime();
		first = false;
		glfwSetTime(0.0);

		system.update(dt);

		system.draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwSetWindowUserPointer(window, NULL);
}