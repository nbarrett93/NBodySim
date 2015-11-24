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
#include <glm/gtc/constants.hpp>

#include "ShaderMgr.h"
#include "SystemCL.h"
#include "ReadFile.h"
#include "particle.h"

#include "ObjLoader.hpp"

#include "VertexAbstract.h"

static void error_callback(int error, const char* description)
{
	// TODO: make this go to log
	std::cerr << description << std::endl;
}

int main();

void test_abstract_vert();

static void run_main_loop(SystemCL &&cl_state, GLFWwindow *window);

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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void button_callback(GLFWwindow* window, int button, int action, int mods);

int main()
{
	SystemCL cl_state;

	cl_state.Load();

	if (!cl_state.Success())
	{
		// TODO: log failure
		std::cout << "SystemCL.Load(): " << cl_state.ErrorLog();
		std::cin.get();
		return -1;
	}

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	// 2x AA
	glfwWindowHint(GLFW_SAMPLES, 2);
	// OpenGL 4.0
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Gravity Simulator", NULL, NULL);
	if (!window)
	{
		glfwTerminate();

		std::cout << "Error creating window\n";
		std::cin.get();

		return -1;
	}

	glfwMakeContextCurrent(window);

	cl_state.CreateContext(wglGetCurrentContext(), wglGetCurrentDC());

	if (!cl_state.Success())
	{
		std::cout << "System.CL: " << cl_state.ErrorLog();

		glfwDestroyWindow(window);
		glfwTerminate();

		std::cin.get();
		return -1;
	}

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		glfwDestroyWindow(window);
		glfwTerminate();

		std::cout << "Error initializing GLEW\n";
		std::cin.get();

		return -1;
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, button_callback);

	run_main_loop(std::move(cl_state), window);

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}

static void run_main_loop(SystemCL &&cl_state, GLFWwindow *window)
{
	// TODO: take these from config
	const float FieldOfView = glm::pi<float>() / 3.0f,
		AspectRatio = 1280.0f / 720.0f;
	bool succ = false;

	ParticleSystemConfig cfg;

	CameraSettings cam_cfg;
	cam_cfg.CamSensitivity = 0.5;
	cam_cfg.CamVelocity = 3;
	cam_cfg.Window = window;
	cam_cfg.FoV = FieldOfView;
	cam_cfg.AspectRatio = AspectRatio;
	cfg.CamSettings = cam_cfg;

	// dummy values for testing
	cfg.NumParticles = 3;
	// blargh. gross.
	cfg.ParticlePositions = reinterpret_cast<glm::vec4*>(dummy_positions);
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

	// GPU gets hammered without this...
	glfwSwapInterval(1);

	glfwSetWindowUserPointer(window, (void*)&system);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

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

		if (!system.good())
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		}

		system.draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	if (!system.good())
	{
		std::cout << "Error in main loop:\n    " << system.err_log() << std::endl;
		std::cin.get();
	}

	glfwSetWindowUserPointer(window, NULL);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
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