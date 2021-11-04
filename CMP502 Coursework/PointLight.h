#pragma once
#include "Graphics.h"
#include "ConstantBuffer.h"
#include "MatrixBuffer.h"


class PointLight
{
	friend class LightHolder;

public:
	PointLight(Graphics& gfx);

	void Bind(Graphics& gfx, DirectX::XMMATRIX view) const;

private:
	struct PointLightBuffer
	{
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 pos_sun;
		alignas(16) DirectX::XMFLOAT3 color_sun;
		alignas(16) DirectX::XMFLOAT3 pos_torch;		
		alignas(16) DirectX::XMFLOAT3 color_torch;
		float intensity_sun;
		float intensity_torch;
		float attConst;	// attenuation of torch
		float attLin;	// "
		float attQuad;	// "
	};

private:
	PointLightBuffer m_data;
	mutable PixelConstantBuffer<PointLightBuffer> m_lightBuffer;

	Camera m_camera;
	struct LightMatrixBuffer
	{
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
	};
	mutable VertexConstantBuffer<LightMatrixBuffer> m_matrixBuffer;

	Camera* m_pMainCamera;
};

