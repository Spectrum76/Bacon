#pragma once

#include <vector>
#include <string>
#include <d3d11.h>

#include <glm/vec3.hpp>

#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 4

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

	void AddModel(std::string filename);
	void Draw();

	void AddDirLight();
	void AddPointLight();
	void AddSpotLight();

	void Bind();

protected:
	void Update();
	void CreateUBO();

private:

	PointLight PointLights;

	SpotLight SpotLights;

	DirLight Sun;

	ID3D11Buffer* mPointLightsUB;
	ID3D11Buffer* mSpotLightsUB;
	ID3D11Buffer* mSceneUB;

	ID3D11Device* mDeviceRef;
	ID3D11DeviceContext* mDeviceContextRef;
};

