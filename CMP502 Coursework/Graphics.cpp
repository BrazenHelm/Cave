#include "Graphics.h"

#include <array>
#include <vector>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "VertexShader.h"
#include "PixelShader.h"
#include "InputLayout.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Sampler.h"

#include "PostProcessor.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3Dcompiler.lib")

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;


Graphics::Graphics(HWND hWnd, int width, int height) :
	m_width(width), m_height(height)
{
	// create device, front/back buffers, swap chain and context
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferDesc.Width = 0;
	scd.BufferDesc.Height = 0;
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 0;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = 0;

	D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
		&scd, &m_pSwapChain, &m_pDevice, nullptr, &m_pContext
	);

	// gain access to texture subresource in swap chain (back buffer)
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
	m_pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &m_pTarget);

	// create depth stencil state
	wrl::ComPtr<ID3D11DepthStencilState> pDepthStencilState;
	D3D11_DEPTH_STENCIL_DESC dssd = {};
	dssd.DepthEnable = TRUE;
	dssd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssd.DepthFunc = D3D11_COMPARISON_LESS;
	m_pDevice->CreateDepthStencilState(&dssd, &pDepthStencilState);
	// bind depth stencil state
	m_pContext->OMSetDepthStencilState(pDepthStencilState.Get(), 1);

	// create depth stencil texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencilTexture;
	D3D11_TEXTURE2D_DESC dstd = {};
	dstd.Width = width;
	dstd.Height = height;
	dstd.MipLevels = 1;
	dstd.ArraySize = 1;
	dstd.Format = DXGI_FORMAT_D32_FLOAT;
	dstd.SampleDesc.Count = 1;
	dstd.SampleDesc.Quality = 0;
	dstd.Usage = D3D11_USAGE_DEFAULT;
	dstd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	m_pDevice->CreateTexture2D(&dstd, nullptr, &pDepthStencilTexture);

	// create depth stencil texture view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	m_pDevice->CreateDepthStencilView(pDepthStencilTexture.Get(), &dsvd, &m_pDSV);

	// create alpha blend state
	D3D11_BLEND_DESC bd = {};
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_pDevice->CreateBlendState(&bd, &m_pBlendState);
	m_pContext->OMSetBlendState(m_pBlendState.Get(), NULL, 0xffffffff);

	// set viewport
	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pContext->RSSetViewports(1, &vp);

	// set up textures with render target views and shader resource views
	D3D11_TEXTURE2D_DESC td = {};
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	m_pDevice->CreateTexture2D(&td, nullptr, &m_pTex_shadow);
	m_pDevice->CreateTexture2D(&td, nullptr, &m_pTex_scene);
	m_pDevice->CreateTexture2D(&td, nullptr, &m_pTex_fog);

	D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
	rtvd.Format = td.Format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	m_pDevice->CreateRenderTargetView(m_pTex_shadow.Get(), &rtvd, &m_pRTV_shadow);
	m_pDevice->CreateRenderTargetView(m_pTex_scene.Get(), &rtvd, &m_pRTV_scene);
	m_pDevice->CreateRenderTargetView(m_pTex_fog.Get(), &rtvd, &m_pRTV_fog);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
	srvd.Format = td.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	srvd.Texture2D.MostDetailedMip = 0;
	m_pDevice->CreateShaderResourceView(m_pTex_shadow.Get(), &srvd, &m_pSRV_shadow);
	m_pDevice->CreateShaderResourceView(m_pTex_scene.Get(), &srvd, &m_pSRV_scene);
	m_pDevice->CreateShaderResourceView(m_pTex_fog.Get(), &srvd, &m_pSRV_fog);

	// set up camera
	m_camera.SetPosition(9.5f, 9.5f, 2.5f);
	m_camera.SetPitchYaw(0.f, -90.f);

	m_pPostProcessor = new PostProcessor(*this, width, height);
}


Graphics::~Graphics() = default;


void Graphics::EndFrame()
{
	m_pSwapChain->Present(1u, 0u);
}


void Graphics::ClearBuffer(float r, float g, float b)
{
	const float color[] = { r, g, b, 1.f };
	m_pContext->ClearRenderTargetView(m_pTarget.Get(), color);
	m_pContext->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
}


void Graphics::DrawIndexed(UINT count)
{
	m_pContext->DrawIndexed(count, 0, 0);
}


DirectX::XMMATRIX Graphics::GetCameraMatrix()
{
	return m_camera.GetView();
}


DirectX::XMMATRIX Graphics::GetProjection() const
{
	//return dx::XMMatrixPerspectiveLH(1.f, 0.5625f, 0.5f, 50.f);
	return dx::XMMatrixPerspectiveLH(0.1f, 0.05625f, 0.05f, 50.f);
}


void Graphics::SetDrawModeDepth()
{
	m_pContext->OMSetRenderTargets(1, m_pRTV_shadow.GetAddressOf(), m_pDSV.Get());
	const float color[] = { 1.f, 1.f, 1.f, 1.f };
	m_pContext->ClearRenderTargetView(m_pRTV_shadow.Get(), color);
	m_pContext->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
}


void Graphics::SetDrawModeScene()
{
	m_pContext->OMSetRenderTargets(1, m_pRTV_scene.GetAddressOf(), m_pDSV.Get());
	const float color[] = { 1.f, 1.f, 1.f, 1.f };
	m_pContext->ClearRenderTargetView(m_pRTV_scene.Get(), color);
	m_pContext->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
	m_pContext->PSSetShaderResources(1, 1, m_pSRV_shadow.GetAddressOf());
}


void Graphics::SetDrawModeFog()
{
	// We render to a new texture which is cleared to have a black background instead of white
	// We don't reset the depth map so that we still hide fog behind objects
	m_pContext->OMSetRenderTargets(1, m_pRTV_fog.GetAddressOf(), m_pDSV.Get());
	const float color[] = { 0.f, 0.f, 0.f, 1.f };
	m_pContext->ClearRenderTargetView(m_pRTV_fog.Get(), color);
}


void Graphics::DoPostProcessing()
{
	m_pPostProcessor->DoBloom(*this, m_pSRV_scene, m_pSRV_fog, m_pTarget);
}


void Graphics::ControlCamera(Inputs inputs, float dt)
{
	m_camera.Control(inputs, dt);
}
