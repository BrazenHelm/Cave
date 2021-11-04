#pragma once

#include "WindowsInclude.h"
#include "Camera.h"
#include "Inputs.h"
//#include "ConstantBuffer.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <wrl.h>


class Graphics
{
	friend class Bindable;
	friend class PostProcessor;

public:
	Graphics(HWND hWnd, int width, int height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();

	void EndFrame();
	void ClearBuffer(float r, float g, float b);

	void DrawIndexed(UINT count);

	DirectX::XMMATRIX GetCameraMatrix();
	DirectX::XMMATRIX GetProjection() const;
	Camera* GetCamera() { return &m_camera; }

	void SetDrawModeDepth();
	void SetDrawModeScene();
	void SetDrawModeFog();
	void DoPostProcessing();

	void ControlCamera(Inputs inputs, float dt);


private:
	int m_width, m_height;

	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		m_pContext;
	Microsoft::WRL::ComPtr<ID3D11Device>			m_pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			m_pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	m_pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	m_pDSV;
	Microsoft::WRL::ComPtr<ID3D11BlendState>		m_pBlendState;

	// Render to texture used for shadow map
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_pTex_shadow;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_pRTV_shadow;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pSRV_shadow;

	// Render to texture used for post-processing
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_pTex_scene;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_pRTV_scene;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pSRV_scene;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_pTex_fog;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_pRTV_fog;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pSRV_fog;
	

	Camera m_camera;

	class PostProcessor* m_pPostProcessor;
};

