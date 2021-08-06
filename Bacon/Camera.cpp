#include "Camera.h"
#include "KeyCodes.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context) : mDeviceRef(device), mDeviceContextRef(context)
{
	position = glm::vec3(15.0f, 2.0f, -0.65f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	yaw = 180.0f;
	pitch = 0.0f;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	mData.View = glm::mat4(1.0f);
	mData.Proj = glm::mat4(1.0f);

	RECT rect;
	GetClientRect(hwnd, &rect);

	float mWindowWidth = (float)(rect.right - rect.left);
	float mWindowHeight = (float)(rect.bottom - rect.top);

	mData.Proj = glm::perspectiveFov(glm::radians(45.0f), mWindowWidth, mWindowHeight, 0.1f, 1000.0f);

	moveSpeed = 5.0f;
	turnSpeed = 0.2f;

	Update();
	CreateUBO();
}

void Camera::Bind()
{
	mDeviceContextRef->VSSetConstantBuffers(1, 1, &mUniformBuffer);
	mDeviceContextRef->PSSetConstantBuffers(0, 1, &mUniformBuffer);
}

void Camera::KeyControl(bool* keys, float deltaTime)
{
	float velocity = moveSpeed * deltaTime;

	if (keys[VK_KEY_W])
	{
		position += front * velocity;
	}

	if (keys[VK_KEY_S])
	{
		position -= front * velocity;
	}

	if (keys[VK_KEY_A])
	{
		position -= right * velocity;
	}

	if (keys[VK_KEY_D])
	{
		position += right * velocity;
	}
}

void Camera::MouseControl(float xChange, float yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	Update();
}

void Camera::CalculateViewMatrix()
{
	mData.View = glm::lookAt(position, position + front, up);
	mData.Eye = position;

	D3D11_MAPPED_SUBRESOURCE cbRes;
	mDeviceContextRef->Map(mUniformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbRes);
	CopyMemory(cbRes.pData, &mData, sizeof(CameraUBO));
	mDeviceContextRef->Unmap(mUniformBuffer, 0);
}

Camera::~Camera()
{
	mUniformBuffer->Release();
}

void Camera::Update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

void Camera::CreateUBO()
{
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));

	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.ByteWidth = (sizeof(CameraUBO) + 255) & ~255;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	mDeviceRef->CreateBuffer(&cbDesc, nullptr, &mUniformBuffer);
}
