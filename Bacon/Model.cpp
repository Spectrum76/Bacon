#include "Model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <glm/gtc/matrix_transform.hpp>

Model::Model(ID3D11Device* device, ID3D11DeviceContext* context) : mDeviceRef(device), mDeviceContextRef(context)
{
	mUniformBuffer = nullptr;

	mData.FModel = glm::mat4(1.0f);
	mData.iModel = glm::mat4(1.0f);

	CreateUBO();
	Update();
}

Model::~Model()
{
	mUniformBuffer->Release();
}

void Model::Draw()
{
	mDeviceContextRef->VSSetConstantBuffers(0, 1, &mUniformBuffer);

	for (auto Meshlet : MeshComponent)
	{
		Meshlet.second->Bind(0);
		Meshlet.first->Draw();
	}
}

void Model::Load(std::string filename)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str());

	for (const auto& shape : shapes)
	{
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;

		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex;

			vertex.Position =
			{
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.TexCoord =
			{
					   attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.Normal =
			{
				-(attrib.normals[3 * index.normal_index + 0]),
				-(attrib.normals[3 * index.normal_index + 1]),
				-(attrib.normals[3 * index.normal_index + 2])
			};

			Vertices.push_back(vertex);
			Indices.push_back((uint32_t)Indices.size());
		}

		Texture* texture = new Texture(mDeviceRef, mDeviceContextRef);
		texture->Init(materials[shape.mesh.material_ids[0]].diffuse_texname);

		Mesh* mesh = new Mesh(mDeviceRef, mDeviceContextRef);
		mesh->Init(Vertices, Indices);

		MeshComponent.push_back(std::make_pair(mesh, texture));
	}
}

void Model::Position(glm::vec3 pos)
{
	mData.FModel = glm::translate(mData.FModel, pos);
	mData.iModel = glm::inverse(mData.FModel);
	Update();
}

void Model::Rotation(glm::vec3 axis, float angle)
{
	mData.FModel = glm::rotate(mData.FModel, angle, axis);
	mData.iModel = glm::inverse(mData.FModel);
	Update();
}

void Model::Scale(glm::vec3 scale)
{
	mData.FModel = glm::scale(mData.FModel, scale);
	mData.iModel = glm::inverse(mData.FModel);
	Update();
}

void Model::Update()
{
	D3D11_MAPPED_SUBRESOURCE cbRes;
	mDeviceContextRef->Map(mUniformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbRes);
	CopyMemory(cbRes.pData, &mData, sizeof(Transform));
	mDeviceContextRef->Unmap(mUniformBuffer, 0);
}

void Model::CreateUBO()
{
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));

	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.ByteWidth = (sizeof(Transform) + 255) & ~255;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	mDeviceRef->CreateBuffer(&cbDesc, nullptr, &mUniformBuffer);
}
