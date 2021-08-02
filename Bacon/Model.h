#pragma once

#include <vector>
#include <string>
#include <d3d11.h>

#include <glm/glm.hpp>

#include "Mesh.h"
#include "Texture.h"

struct Transform
{
	glm::mat4 FModel;
	glm::mat4 iModel;
};

class Model
{
public:
	Model(ID3D11Device* device, ID3D11DeviceContext* context);
	~Model();

	void Draw();
	void Load(std::string filename);

	void Position(glm::vec3 pos);
	void Rotation(glm::vec3 axis, float angle);
	void Scale(glm::vec3 scale);

protected:
	void Update();
	void CreateUBO();

private:
	std::vector<std::pair<Mesh*, Texture*>> MeshComponent;
	Transform mData;

	ID3D11Buffer* mUniformBuffer;

	ID3D11Device* mDeviceRef;
	ID3D11DeviceContext* mDeviceContextRef;
};

