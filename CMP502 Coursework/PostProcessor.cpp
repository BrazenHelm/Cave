#include "PostProcessor.h"

#include <vector>

#include "IndexBuffer.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "Sampler.h"
#include "VertexBuffer.h"
#include "VertexShader.h"


PostProcessor::PostProcessor(Graphics& gfx, int width, int height) :
	m_width(width), m_height(height)
{
	CreateRTT(gfx, m_pTex_1, m_pRTV_1, m_pSRV_1, 1);
	CreateRTT(gfx, m_pTex_2_1, m_pRTV_2_1, m_pSRV_2_1, 2);
	CreateRTT(gfx, m_pTex_2_2, m_pRTV_2_2, m_pSRV_2_2, 2);
	CreateRTT(gfx, m_pTex_4_1, m_pRTV_4_1, m_pSRV_4_1, 4);
	CreateRTT(gfx, m_pTex_4_2, m_pRTV_4_2, m_pSRV_4_2, 4);
	CreateRTT(gfx, m_pTex_8_1, m_pRTV_8_1, m_pSRV_8_1, 8);
	CreateRTT(gfx, m_pTex_8_2, m_pRTV_8_2, m_pSRV_8_2, 8);

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{"Position2D", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA}
	};
	m_fullscreen_vs = std::make_unique<VertexShader>(gfx, "Shaders/postProcess_vs.cso");
	m_fullscreen_inputLayout = std::make_unique<InputLayout>(
		gfx, ied, m_fullscreen_vs->GetBytecode());

	m_threshold_ps = std::make_unique<PixelShader>(gfx, "Shaders/threshold_ps.cso");
	m_blurHorizontal_ps = std::make_unique<PixelShader>(gfx, "Shaders/blur_horizontal_ps.cso");
	m_blurVertical_ps = std::make_unique<PixelShader>(gfx, "Shaders/blur_vertical_ps.cso");
	m_bloomCombine_ps = std::make_unique<PixelShader>(gfx, "Shaders/bloom_combine_ps.cso");

	std::vector<DirectX::XMFLOAT2> vertices
	{
		{-1, 1}, {1, 1}, {-1, -1}, {1, -1}
	};
	std::vector<unsigned short> indices{ 0, 1, 2, 1, 3, 2 };

	m_fullscreen_vertexBuffer = std::make_unique<VertexBuffer>(gfx, vertices);
	m_fullscreen_indexBuffer = std::make_unique<IndexBuffer>(gfx, indices);

	m_sampler = std::make_unique<Sampler>(gfx);
}


PostProcessor::~PostProcessor() = default;


void PostProcessor::DoBloom(Graphics& gfx,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSceneSRV,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pFogSRV,
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pScreenRTV)
{
	const float clearColor[] = { 1.f, 1.f, 1.f, 1.f };

	SetVPForDownsample(gfx, 1);

	m_fullscreen_vs->Bind(gfx);
	m_fullscreen_inputLayout->Bind(gfx);
	m_fullscreen_vertexBuffer->Bind(gfx);
	m_fullscreen_indexBuffer->Bind(gfx);

	m_sampler->Bind(gfx);

	// Blur fog at 1/2 res
	SetVPForDownsample(gfx, 2);

	SetRenderTarget(gfx, m_pRTV_2_1, 1.f, 1.f, 1.f);
	m_threshold_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, pFogSRV.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	SetRenderTarget(gfx, m_pRTV_2_2, 1.f, 1.f, 1.f);
	m_blurHorizontal_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, m_pSRV_2_1.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	SetRenderTarget(gfx, m_pRTV_2_1, 1.f, 1.f, 1.f);
	m_blurVertical_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, m_pSRV_2_2.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	// Blur fog at 1/4 res
	SetVPForDownsample(gfx, 4);

	SetRenderTarget(gfx, m_pRTV_4_1, 1.f, 1.f, 1.f);
	m_threshold_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, pFogSRV.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	SetRenderTarget(gfx, m_pRTV_4_2, 1.f, 1.f, 1.f);
	m_blurHorizontal_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, m_pSRV_4_1.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	SetRenderTarget(gfx, m_pRTV_4_1, 1.f, 1.f, 1.f);
	m_blurVertical_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, m_pSRV_4_2.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	// Blur fog at 1/8 res
	SetVPForDownsample(gfx, 8);

	SetRenderTarget(gfx, m_pRTV_8_1, 1.f, 1.f, 1.f);
	m_threshold_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, pFogSRV.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	SetRenderTarget(gfx, m_pRTV_8_2, 1.f, 1.f, 1.f);
	m_blurHorizontal_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, m_pSRV_8_1.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	SetRenderTarget(gfx, m_pRTV_8_1, 1.f, 1.f, 1.f);
	m_blurVertical_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, m_pSRV_8_2.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	// Combine scene with the three versions of blurred fog
	SetVPForDownsample(gfx, 1);

	SetRenderTarget(gfx, m_pRTV_1, 1.f, 1.f, 1.f);
	m_bloomCombine_ps->Bind(gfx);	
	GetContext(gfx)->PSSetShaderResources(0, 1, pSceneSRV.GetAddressOf());
	GetContext(gfx)->PSSetShaderResources(1, 1, m_pSRV_2_1.GetAddressOf());
	GetContext(gfx)->PSSetShaderResources(2, 1, m_pSRV_4_1.GetAddressOf());
	GetContext(gfx)->PSSetShaderResources(3, 1, m_pSRV_8_1.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	// Blur scene at 1/2 res
	SetVPForDownsample(gfx, 2);

	SetRenderTarget(gfx, m_pRTV_2_1, 1.f, 1.f, 1.f);
	m_threshold_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, pSceneSRV.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	SetRenderTarget(gfx, m_pRTV_2_2, 1.f, 1.f, 1.f);
	m_blurHorizontal_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, m_pSRV_2_1.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	SetRenderTarget(gfx, m_pRTV_2_1, 1.f, 1.f, 1.f);
	m_blurVertical_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, m_pSRV_2_2.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	// Blur scene at 1/4 res
	SetVPForDownsample(gfx, 4);

	SetRenderTarget(gfx, m_pRTV_4_1, 1.f, 1.f, 1.f);
	m_threshold_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, pSceneSRV.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	SetRenderTarget(gfx, m_pRTV_4_2, 1.f, 1.f, 1.f);
	m_blurHorizontal_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, m_pSRV_4_1.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	SetRenderTarget(gfx, m_pRTV_4_1, 1.f, 1.f, 1.f);
	m_blurVertical_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, m_pSRV_4_2.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	// Blur scene at 1/8 res
	SetVPForDownsample(gfx, 8);

	SetRenderTarget(gfx, m_pRTV_8_1, 1.f, 1.f, 1.f);
	m_threshold_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, pSceneSRV.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	SetRenderTarget(gfx, m_pRTV_8_2, 1.f, 1.f, 1.f);
	m_blurHorizontal_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, m_pSRV_8_1.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	SetRenderTarget(gfx, m_pRTV_8_1, 1.f, 1.f, 1.f);
	m_blurVertical_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, m_pSRV_8_2.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);

	// Recombine
	SetVPForDownsample(gfx, 1);

	SetRenderTarget(gfx, pScreenRTV, 1.f, 1.f, 1.f);
	m_bloomCombine_ps->Bind(gfx);
	GetContext(gfx)->PSSetShaderResources(0, 1, m_pSRV_1.GetAddressOf());
	GetContext(gfx)->PSSetShaderResources(1, 1, m_pSRV_2_1.GetAddressOf());
	GetContext(gfx)->PSSetShaderResources(2, 1, m_pSRV_4_1.GetAddressOf());
	GetContext(gfx)->PSSetShaderResources(3, 1, m_pSRV_8_1.GetAddressOf());
	GetContext(gfx)->DrawIndexed(m_fullscreen_indexBuffer->GetCount(), 0u, 0u);
}


void PostProcessor::CreateRTT(
	Graphics& gfx,
	Microsoft::WRL::ComPtr<ID3D11Texture2D>& pTex,
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& pRTV,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& pSRV,
	int downsampling
) {
	D3D11_TEXTURE2D_DESC td = {};
	td.Width = m_width / downsampling;
	td.Height = m_height / downsampling;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	GetDevice(gfx)->CreateTexture2D(&td, nullptr, &pTex);

	D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
	rtvd.Format = td.Format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	GetDevice(gfx)->CreateRenderTargetView(pTex.Get(), &rtvd, &pRTV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
	srvd.Format = td.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	srvd.Texture2D.MostDetailedMip = 0;
	GetDevice(gfx)->CreateShaderResourceView(pTex.Get(), &srvd, &pSRV);
}


void PostProcessor::SetVPForDownsample(Graphics& gfx, int downsampling)
{
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(m_width / downsampling);
	vp.Height = static_cast<float>(m_height / downsampling);
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	GetContext(gfx)->RSSetViewports(1, &vp);
}


void PostProcessor::SetRenderTarget(Graphics& gfx,
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRTV,
	float r, float g, float b
) {
	const float clearColor[] = { r, g, b, 1.f };
	GetContext(gfx)->OMSetRenderTargets(1, pRTV.GetAddressOf(), GetDSV(gfx));
	GetContext(gfx)->ClearRenderTargetView(pRTV.Get(), clearColor);
	GetContext(gfx)->ClearDepthStencilView(GetDSV(gfx), D3D11_CLEAR_DEPTH, 1.f, 0);
}


ID3D11DeviceContext* PostProcessor::GetContext(Graphics& gfx)
{
	return gfx.m_pContext.Get();
}


ID3D11Device* PostProcessor::GetDevice(Graphics& gfx)
{
	return gfx.m_pDevice.Get();
}


ID3D11DepthStencilView* PostProcessor::GetDSV(Graphics& gfx)
{
	return gfx.m_pDSV.Get();
}
