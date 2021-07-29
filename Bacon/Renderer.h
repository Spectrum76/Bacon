#pragma once

#include <d3d11.h>
#include <dxgi1_4.h>

class Renderer
{
public:
	Renderer(HWND hwnd);
	~Renderer();

	void Init();
	void Render();
	void Update();
	void Destroy();

protected:
	void InitAPI();
	void InitSwapChain();
	void InitFrameBuffer();
	void InitPipeline();
	void LoadAssets();

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

	ID3D11Texture2D* mDSBuffer;
	ID3D11DepthStencilView* mDSView;
	ID3D11DepthStencilState* mDSState;

	ID3D11InputLayout* mInputLayout;
	ID3D11RasterizerState* mRasterState;
	ID3D11SamplerState* mSamplerState;

	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader* mPixelShader;
};

