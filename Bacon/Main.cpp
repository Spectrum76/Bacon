// Copyright (c) 2021 Rayvant. All rights reserved.

#define WIN32_LEAN_AND_MEAN
#define GLFW_EXPOSE_NATIVE_WIN32

#include <Windows.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"

bool Keys[1024];

double lastX;
double lastY;

double xChange;
double yChange;

bool mouseFirstMoved = true;

double deltaTime = 0.0f;
double lastTime = 0.0f;

void HandleKeys(GLFWwindow* window, int Key, int Code, int Action, int Mode);
void HandleMouse(GLFWwindow* window, double xPos, double yPos);

int main()
{
	const int WIDTH = 800;
	const int HEIGHT = 600;

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Bacon", nullptr, nullptr);

	glfwSetKeyCallback(window, HandleKeys);
	glfwSetCursorPosCallback(window, HandleMouse);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Renderer renderer = Renderer(glfwGetWin32Window(window));

	renderer.Init();

	Scene scene = Scene(renderer.GetDevice(), renderer.GetContext());

	Camera camera = Camera(glfwGetWin32Window(window), renderer.GetDevice(), renderer.GetContext());

	scene.AddModel("Sphere.obj")->Position(glm::vec3(0.0f, 5.0f, 0.0f));
	scene.AddModel("Dabrovic_Sponza.obj");
	scene.AddModel("Grid.obj")->Position(glm::vec3(0.0f, -1.0f, 0.0f));

	while (!glfwWindowShouldClose(window))
	{
		double now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		camera.KeyControl(Keys, (float)deltaTime);
		camera.MouseControl((float)xChange, (float)yChange);

		xChange = 0.0f;
		yChange = 0.0f;

		renderer.ExecShadowPass();

		scene.BindLSMatrix();

		scene.Draw();

		renderer.ExecForwardPass();

		camera.CalculateViewMatrix();

		camera.Bind();

		scene.BindLSMatrix();

		scene.Bind();

		scene.Draw();

		renderer.ExecGBufferPass();

		camera.Bind();

		scene.Draw();

		renderer.Update();

		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}

void HandleKeys(GLFWwindow* window, int Key, int Code, int Action, int Mode)
{
	if (Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (Key >= 0 && Key < 1024)
	{
		if (Action == GLFW_PRESS)
		{
			Keys[Key] = true;
		}
		else if (Action == GLFW_RELEASE)
		{
			Keys[Key] = false;
		}
	}
}

void HandleMouse(GLFWwindow* window, double xPos, double yPos)
{
	if (mouseFirstMoved)
	{
		lastX = xPos;
		lastY = yPos;
		mouseFirstMoved = false;
	}

	xChange = xPos - lastX;
	yChange = lastY - yPos;

	lastX = xPos;
	lastY = yPos;
}