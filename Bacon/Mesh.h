#pragma once

#include <vector>
#include <d3d11.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TexCoord;
	glm::vec3 Normal;
};

class Mesh
{
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* context);
	~Mesh();

	void Init(std::vector<Vertex> Vertices, std::vector<uint32_t> Indices);
	void Draw();

protected:
	void CreateVBO(std::vector<Vertex> Vertices);
	void CreateIBO(std::vector<uint32_t> Indices);

private:
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	UINT IndexCount;

	UINT Stride;
	UINT Offset;

	ID3D11Device* mDeviceRef;
	ID3D11DeviceContext* mDeviceContextRef;
};

