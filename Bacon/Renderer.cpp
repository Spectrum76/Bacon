#include "Renderer.h"
#include "Utilities.h"

Renderer::Renderer(HWND hwnd) : mHwnd(hwnd)
{
	RECT rect;
	GetClientRect(hwnd, &rect);

	mWindowWidth = rect.right - rect.left;
	mWindowHeight = rect.bottom - rect.top;

	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = static_cast<float>(mWindowWidth);
	mViewport.Height = static_cast<float>(mWindowHeight);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	mSHDViewport.TopLeftX = 0.0f;
	mSHDViewport.TopLeftY = 0.0f;
	mSHDViewport.Width = static_cast<float>(4096);
	mSHDViewport.Height = static_cast<float>(4096);
	mSHDViewport.MinDepth = 0.0f;
	mSHDViewport.MaxDepth = 1.0f;
}

Renderer::~Renderer()
{
	Destroy();
}

void Renderer::Init()
{
	InitAPI();
	InitSwapChain();
	InitPrepass();
	InitShadowPass();
}

void Renderer::ExecPrepass()
{
	mDeviceContext->VSSetShader(mForwardVS, 0, 0);
	mDeviceContext->PSSetShader(mForwardPS, 0, 0);

	mDeviceContext->PSSetSamplers(0, 1, &mSamplerState);
	mDeviceContext->PSSetSamplers(1, 1, &mSHDSamplerState);

	mDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	mDeviceContext->PSSetShaderResources(1, 1, &mSHDMapSRV);

	mDeviceContext->IASetInputLayout(mInputLayout);
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mDeviceContext->RSSetViewports(1, &mViewport);
	mDeviceContext->RSSetState(mRasterState);

	mDeviceContext->OMSetRenderTargets(1, &mRTV, mDSView);
	mDeviceContext->OMSetDepthStencilState(mDSState, 1);

	const float clearColor[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	mDeviceContext->ClearRenderTargetView(mRTV, clearColor);
	mDeviceContext->ClearDepthStencilView(mDSView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::Update()
{
	mSwapchain->Present(0, 0);
}

void Renderer::Destroy()
{
	mInputLayout->Release();
	mRasterState->Release();
	mSamplerState->Release();

	mForwardVS->Release();
	mForwardPS->Release();

	mDSBuffer->Release();
	mDSState->Release();
	mDSView->Release();

	mRTV->Release();
	mRenderTarget->Release();

	mSwapchain->Release();

	mDeviceContext->Release();
	mDevice->Release();

	mFactory->Release();
}

void Renderer::ExecShadowPass()
{
	mDeviceContext->VSSetShader(mShadowVS, 0, 0);
	mDeviceContext->PSSetShader(nullptr, 0, 0);

	mDeviceContext->IASetInputLayout(mInputLayout);
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mDeviceContext->RSSetViewports(1, &mSHDViewport);
	mDeviceContext->RSSetState(mSHDRasterState);

	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	mDeviceContext->PSSetShaderResources(1, 1, nullSRV);

	mDeviceContext->OMSetRenderTargets(0, nullptr, mSHDMapDSV);
	mDeviceContext->OMSetDepthStencilState(mSHDState, 1);

	mDeviceContext->ClearDepthStencilView(mSHDMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11Device* Renderer::GetDevice()
{
	return mDevice;
}

ID3D11DeviceContext* Renderer::GetContext()
{
	return mDeviceContext;
}

void Renderer::InitAPI()
{
	UINT dxgiFactoryFlags = 0;
	UINT d3d11DeviceFlags = 0;

#ifdef _DEBUG
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&mFactory));

	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		NULL, d3d11DeviceFlags,
		NULL, NULL, D3D11_SDK_VERSION,
		&mDevice, nullptr, &mDeviceContext);
}

void Renderer::InitSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	swapchainDesc.BufferCount = 2;
	swapchainDesc.Width = mWindowWidth;
	swapchainDesc.Height = mWindowHeight;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.SampleDesc.Count = 1;

	mFactory->CreateSwapChainForHwnd(mDevice, mHwnd, &swapchainDesc, nullptr, nullptr, &mSwapchain);
}

void Renderer::InitPrepass()
{
	InitPrepassFB();
	InitPrepassPSO();
}

void Renderer::InitShadowPass()
{
	InitShadowPassFB();
	InitShadowPassPSO();
}

void Renderer::InitPrepassFB()
{
	mSwapchain->GetBuffer(0, IID_PPV_ARGS(&mRenderTarget));

	mDevice->CreateRenderTargetView(mRenderTarget, nullptr, &mRTV);

	D3D11_TEXTURE2D_DESC dSDesc{};
	dSDesc.Width = mWindowWidth;
	dSDesc.Height = mWindowHeight;
	dSDesc.MipLevels = 1;
	dSDesc.ArraySize = 1;
	dSDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dSDesc.SampleDesc.Count = 1;
	dSDesc.SampleDesc.Quality = 0;
	dSDesc.Usage = D3D11_USAGE_DEFAULT;
	dSDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	mDevice->CreateTexture2D(&dSDesc, nullptr, &mDSBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV{};
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	mDevice->CreateDepthStencilView(mDSBuffer, &descDSV, &mDSView);
}

void Renderer::InitPrepassPSO()
{
	D3D11_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
	  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	  { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	  { "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3D11_RASTERIZER_DESC rasterDesc{};
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;

	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	D3D11_DEPTH_STENCIL_DESC dssDesc{};
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS;

	mDevice->CreateDepthStencilState(&dssDesc, &mDSState);

	auto VSBytecode = Read("ForwardVS.cso");
	auto PSBytecode = Read("ForwardPS.cso");

	mDevice->CreateVertexShader(VSBytecode.data(), VSBytecode.size(), nullptr, &mForwardVS);
	mDevice->CreatePixelShader(PSBytecode.data(), PSBytecode.size(), nullptr, &mForwardPS);

	mDevice->CreateSamplerState(&samplerDesc, &mSamplerState);
	mDevice->CreateRasterizerState(&rasterDesc, &mRasterState);
	mDevice->CreateInputLayout(inputElementDescs, _countof(inputElementDescs), VSBytecode.data(), VSBytecode.size(), &mInputLayout);
}

void Renderer::InitShadowPassFB()
{
	D3D11_TEXTURE2D_DESC dSDesc{};
	dSDesc.Width = 4096;
	dSDesc.Height = 4096;
	dSDesc.MipLevels = 1;
	dSDesc.ArraySize = 1;
	dSDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	dSDesc.SampleDesc.Count = 1;
	dSDesc.SampleDesc.Quality = 0;
	dSDesc.Usage = D3D11_USAGE_DEFAULT;
	dSDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	mDevice->CreateTexture2D(&dSDesc, nullptr, &mSHDMap);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV{};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	mDevice->CreateDepthStencilView(mSHDMap, &descDSV, &mSHDMapDSV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	mDevice->CreateShaderResourceView(mSHDMap, &srvDesc, &mSHDMapSRV);
}

void Renderer::InitShadowPassPSO()
{
	D3D11_RASTERIZER_DESC rasterDesc{};
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;

	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	D3D11_DEPTH_STENCIL_DESC dssDesc{};
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS;

	mDevice->CreateDepthStencilState(&dssDesc, &mSHDState);

	mDevice->CreateSamplerState(&samplerDesc, &mSHDSamplerState);
	mDevice->CreateRasterizerState(&rasterDesc, &mSHDRasterState);

	auto VSBytecode = Read("ShadowVS.cso");
	mDevice->CreateVertexShader(VSBytecode.data(), VSBytecode.size(), nullptr, &mShadowVS);
}
