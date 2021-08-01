// Copyright (c) 2021 Rayvant. All rights reserved.

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <chrono>

#include "Renderer.h"
#include "Camera.h"
#include "Model.h"

using namespace std::chrono;

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

bool Keys[255];

double lastX;
double lastY;

double xChange;
double yChange;

bool mouseFirstMoved = true;

double deltaTime = 0.0f;
double lastTime = 0.0f;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	const int WIDTH = 800;
	const int HEIGHT = 600;

	WNDCLASSEX windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WndProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"WindowClass";

	RegisterClassEx(&windowClass);

	RECT windowRect = { 0, 0, WIDTH, HEIGHT };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hwnd = CreateWindow(
		L"WindowClass",
		L"Bacon",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr,
		hInstance,
		nullptr);

	ShowWindow(hwnd, nCmdShow);

	Renderer renderer = Renderer(hwnd);

	renderer.Init();

	Model cube = Model(renderer.GetDevice(), renderer.GetContext());
	Camera camera = Camera(hwnd, renderer.GetDevice(), renderer.GetContext());

	cube.Load("cube.obj");

	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			auto before = std::chrono::high_resolution_clock::now();

			camera.KeyControl(Keys, (float)deltaTime);
			camera.MouseControl((float)xChange, (float)yChange);

			xChange = 0.0f;
			yChange = 0.0f;

			camera.CalculateViewMatrix();

			renderer.Render();

			camera.Bind();

			cube.Draw();

			renderer.Update();

			auto after = std::chrono::high_resolution_clock::now();

			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(after - before);
			deltaTime = (float)duration.count() / 1000000.0f;
		}
	}

	return static_cast<char>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
		Keys[wParam] = true;
		break;

	case WM_KEYUP:
		Keys[wParam] = false;
		break;

	case WM_MOUSEMOVE:
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

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

	return DefWindowProc(hWnd, message, wParam, lParam);
}