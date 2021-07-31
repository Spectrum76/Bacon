#pragma once

#include <d3d11.h>
#include <glm/glm.hpp>

struct CameraUBO
{
	glm::mat4 View;
	glm::mat4 Proj;
	glm::vec3 Eye;
};

class Camera
{
public:
	Camera(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);

	void Bind();

	void KeyControl(bool* keys, float deltaTime);
	void MouseControl(float xChange, float yChange);

	void CalculateViewMatrix();

	~Camera();

protected:
	void Update();
	void CreateUBO();

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	ID3D11Device* mDeviceRef;
	ID3D11DeviceContext* mDeviceContextRef;

	ID3D11Buffer* mUniformBuffer;

	CameraUBO mData;

	float yaw;
	float pitch;

	float moveSpeed;
	float turnSpeed;
};

