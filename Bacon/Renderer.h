#pragma once

#include <d3d11.h>
#include <dxgi1_4.h>

class Renderer
{
public:
	Renderer(HWND hwnd);
	~Renderer();

	void Init();
	void Update();
	void Destroy();

	void ExecForwardPass();
	void ExecShadowPass();
	void ExecGBufferPass();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();

protected:
	void InitAPI();
	void InitSwapChain();

	void InitPipeline();
	void InitFrameBuffer();

	void InitShadowPass();

private:
	HWND mHwnd;

	int mWindowWidth;
	int mWindowHeight;

	IDXGIFactory4* mFactory;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;

	IDXGISwapChain1* mSwapchain;
	D3D11_VIEWPORT mViewport;

	ID3D11Texture2D* mRenderTarget;
	ID3D11RenderTargetView* mRTV;

	ID3D11Texture2D* gAlbedo;
	ID3D11Texture2D* gNormal;
	ID3D11Texture2D* gDepth;

	ID3D11RenderTargetView* gAlbedoRTV;
	ID3D11RenderTargetView* gNormalRTV;
	ID3D11DepthStencilView* gDepthView;

	ID3D11ShaderResourceView* gAlbedoSRV;
	ID3D11ShaderResourceView* gNormalSRV;
	ID3D11ShaderResourceView* gDepthSRV;

	ID3D11InputLayout* mInputLayout;
	ID3D11SamplerState* mSamplerState;
	ID3D11RasterizerState* mRasterState;
	ID3D11DepthStencilState* mDepthSState;

	ID3D11Texture2D* mSHDMap;
	ID3D11DepthStencilView* mSHDMapDSV;
	ID3D11ShaderResourceView* mSHDMapSRV;

	ID3D11RasterizerState* mSHDRasterState;
	ID3D11SamplerState* mSHDSamplerState;
	ID3D11DepthStencilState* mSHDState;
	D3D11_VIEWPORT mSHDViewport;

	ID3D11VertexShader* mShadowVS;

	ID3D11VertexShader* mGBufferVS;
	ID3D11PixelShader* mGBufferPS;

	ID3D11VertexShader* mForwardVS;
	ID3D11PixelShader* mForwardPS;
};

