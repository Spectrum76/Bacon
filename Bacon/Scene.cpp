#include "Scene.h"

Scene::Scene(ID3D11Device* device, ID3D11DeviceContext* context) : mDeviceRef(device), mDeviceContextRef(context)
{
	mPointLightsUB = nullptr;
	mSpotLightsUB = nullptr;
	mSceneUB = nullptr;

	AddDirLight();
	CreateUBO();
	Update();
}

Scene::~Scene()
{
	mPointLightsUB->Release();
	mSpotLightsUB->Release();
	mSceneUB->Release();
}

void Scene::AddModel(std::string filename)
{
}

void Scene::Draw()
{
}

void Scene::AddDirLight()
{
	Sun.colour = glm::vec3(1.0f, 1.0f, 1.0f);
	Sun.direction = glm::vec3(-50.0f, -50.0f, 0.0f);
	Sun.ambientIntensity = 0.2f;
	Sun.diffuseIntensity = 1.0f;
}

void Scene::AddPointLight()
{
	PointLights.colour = glm::vec3(0.0f, 0.0f, 0.0f);
	PointLights.position = glm::vec3(0.0f, 0.0f, 0.0f);
	PointLights.ambientIntensity = 0.0f;
	PointLights.diffuseIntensity = 0.0f;
	PointLights.constant = 1.0f;
	PointLights.linear = 0.09f;
	PointLights.quadratic = 0.032f;
}

void Scene::AddSpotLight()
{
}

void Scene::Bind()
{
	mDeviceContextRef->PSSetConstantBuffers(1, 1, &mSceneUB);
	mDeviceContextRef->PSSetConstantBuffers(3, 1, &mSpotLightsUB);
	mDeviceContextRef->PSSetConstantBuffers(2, 1, &mPointLightsUB);
}

void Scene::Update()
{
	D3D11_MAPPED_SUBRESOURCE cbRes0;
	mDeviceContextRef->Map(mPointLightsUB, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbRes0);
	CopyMemory(cbRes0.pData, &PointLights, sizeof(PointLights));
	mDeviceContextRef->Unmap(mPointLightsUB, 0);

	D3D11_MAPPED_SUBRESOURCE cbRes1;
	mDeviceContextRef->Map(mSpotLightsUB, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbRes1);
	CopyMemory(cbRes1.pData, &SpotLights, sizeof(SpotLights));
	mDeviceContextRef->Unmap(mSpotLightsUB, 0);

	D3D11_MAPPED_SUBRESOURCE cbRes2;
	mDeviceContextRef->Map(mSceneUB, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbRes2);
	CopyMemory(cbRes2.pData, &Sun, sizeof(DirLight));
	mDeviceContextRef->Unmap(mSceneUB, 0);
}

void Scene::CreateUBO()
{
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.ByteWidth = (sizeof(PointLights) + 255) & ~255;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	mDeviceRef->CreateBuffer(&cbDesc, nullptr, &mPointLightsUB);

	cbDesc.ByteWidth = (sizeof(SpotLights) + 255) & ~255;

	mDeviceRef->CreateBuffer(&cbDesc, nullptr, &mSpotLightsUB);

	cbDesc.ByteWidth = (sizeof(DirLight) + 255) & ~255;

	mDeviceRef->CreateBuffer(&cbDesc, nullptr, &mSceneUB);
}
