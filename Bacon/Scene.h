#pragma once

#include <vector>
#include <string>
#include <d3d11.h>

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"

#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 4

struct LightSpace
{
	glm::mat4 View;
	glm::mat4 Proj;
};

struct DirLight
{
	glm::vec3 colour;

	float _padding;

	glm::vec3 direction;

	float ambientIntensity;
	float diffuseIntensity;
};

struct PointLight
{
	glm::vec3 colour;
	glm::vec3 position;

	float ambientIntensity;
	float diffuseIntensity;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight
{
	glm::vec3 colour;
	glm::vec3 position;
	glm::vec3 direction;

	float procEdge;
	float constant;
	float linear;
	float quadratic;
};

class Scene
{
public:
	Scene(ID3D11Device* device, ID3D11DeviceContext* context);
	~Scene();

	Model* AddModel(std::string filename);
	void Draw();

	void AddDirLight();
	void AddPointLight();
	void AddSpotLight();

	void Bind();
	void BindLSMatrix();

protected:
	void Update();
	void CreateUBO();

private:

	LightSpace LightSpaceData;

	PointLight PointLights;

	SpotLight SpotLights;

	DirLight Sun;

	std::vector<Model*> mSceneModels;

	ID3D11Buffer* mPointLightsUB;
	ID3D11Buffer* mSpotLightsUB;
	ID3D11Buffer* mLightSpaceUB;
	ID3D11Buffer* mSceneUB;

	ID3D11Device* mDeviceRef;
	ID3D11DeviceContext* mDeviceContextRef;
};

