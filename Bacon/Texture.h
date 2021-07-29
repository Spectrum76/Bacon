#pragma once

#include <string>
#include <d3d11.h>

class Texture
{
public:
	Texture(ID3D11Device* device, ID3D11DeviceContext* context);
	~Texture();

	void Init(std::string filepath);
	void Bind(UINT slot);

private:
	ID3D11Texture2D* mTexture;
	ID3D11ShaderResourceView* mSRV;

	ID3D11Device* mDeviceRef;
	ID3D11DeviceContext* mDeviceContextRef;
};

