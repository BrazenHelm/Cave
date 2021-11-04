#pragma once
#include "WindowsInclude.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <wrl.h>

#include "Graphics.h"



class PostProcessor
{
public:
	PostProcessor(Graphics& gfx, int width, int height);
	~PostProcessor();

	void DoBloom(Graphics& gfx,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSceneSRV,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pFogSRV,
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pScreenRTV);

private:
	int m_width, m_height;

	ID3D11DeviceContext* GetContext(Graphics& gfx);
	ID3D11Device* GetDevice(Graphics& gfx);
	ID3D11DepthStencilView* GetDSV(Graphics& gfx);

	void CreateRTT(
		Graphics& gfx,
		Microsoft::WRL::ComPtr<ID3D11Texture2D>& pTex,
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& pRTV,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& pSRV,
		int downsampling
	);

	void SetVPForDownsample(Graphics& gfx, int downsampling);

	void SetRenderTarget(Graphics& gfx,
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRTV,
		float r, float g, float b
	);

	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_pTex_1;	// full res
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_pRTV_1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pSRV_1;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_pTex_2_1;	// 1/2 res
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_pRTV_2_1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pSRV_2_1;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_pTex_2_2;	// 1/2 res
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_pRTV_2_2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pSRV_2_2;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_pTex_4_1;	// 1/4 res
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_pRTV_4_1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pSRV_4_1;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_pTex_4_2;	// 1/4 res
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_pRTV_4_2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pSRV_4_2;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_pTex_8_1;	// 1/8 res
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_pRTV_8_1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pSRV_8_1;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_pTex_8_2;	// 1/8 res
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_pRTV_8_2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pSRV_8_2;

	std::unique_ptr<class VertexShader>		m_fullscreen_vs;
	std::unique_ptr<class InputLayout>		m_fullscreen_inputLayout;
	std::unique_ptr<class VertexBuffer>		m_fullscreen_vertexBuffer;
	std::unique_ptr<class IndexBuffer>		m_fullscreen_indexBuffer;

	std::unique_ptr<class PixelShader>		m_threshold_ps;
	std::unique_ptr<class PixelShader>		m_blurHorizontal_ps;
	std::unique_ptr<class PixelShader>		m_blurVertical_ps;
	std::unique_ptr<class PixelShader>		m_bloomCombine_ps;
	
	std::unique_ptr<class Sampler>			m_sampler;
};

