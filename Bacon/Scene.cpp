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

Model* Scene::AddModel(std::string filename)
{
	Model* model = new Model(mDeviceRef, mDeviceContextRef);
	model->Load(filename);
	mSceneModels.push_back(model);
	return model;
}

void Scene::Draw()
{
	for (auto model : mSceneModels)
	{
		model->Draw();
	}
}

void Scene::AddDirLight()
{
	Sun.colour = glm::vec3(1.0f, 1.0f, 1.0f);
	Sun.direction = glm::vec3(0.0f, -50.0f, 0.0f);
	Sun.ambientIntensity = 0.3f;
	Sun.diffuseIntensity = 0.5f;

	float near_plane = 0.0f, far_plane = 55.5f;
	LightSpaceData.Proj = glm::ortho(-18.0f, 18.0f, -18.0f, 18.0f, near_plane, far_plane);
	LightSpaceData.View = glm::lookAt(-Sun.direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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

void Scene::BindLSMatrix()
{
	mDeviceContextRef->VSSetConstantBuffers(2, 1, &mLightSpaceUB);
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

	D3D11_MAPPED_SUBRESOURCE cbRes3;
	mDeviceContextRef->Map(mLightSpaceUB, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbRes3);
	CopyMemory(cbRes3.pData, &LightSpaceData, sizeof(LightSpace));
	mDeviceContextRef->Unmap(mLightSpaceUB, 0);
}

void Scene::CreateUBO()
{
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.ByteWidth = (sizeof(PointLight) + 255) & ~255;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	mDeviceRef->CreateBuffer(&cbDesc, nullptr, &mPointLightsUB);

	cbDesc.ByteWidth = (sizeof(SpotLight) + 255) & ~255;

	mDeviceRef->CreateBuffer(&cbDesc, nullptr, &mSpotLightsUB);

	cbDesc.ByteWidth = (sizeof(DirLight) + 255) & ~255;

	mDeviceRef->CreateBuffer(&cbDesc, nullptr, &mSceneUB);

	cbDesc.ByteWidth = (sizeof(LightSpace) + 255) & ~255;

	mDeviceRef->CreateBuffer(&cbDesc, nullptr, &mLightSpaceUB);
}
