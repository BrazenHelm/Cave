#include "PointLight.h"


PointLight::PointLight(Graphics& gfx) :
	m_lightBuffer(gfx),
	m_matrixBuffer(gfx, 1)
{
	m_data.ambient = { 0.30f, 0.20f, 0.40f };

	// sun
	m_data.pos_sun = { 9.5f, 13.f, -5.f };
	m_camera.SetPosition(9.5f, 13.f, -5.f);
	m_camera.SetPitchYaw(-30.f, -90.f);
	m_data.color_sun = { 1.f, 1.f, 0.8f };
	m_data.intensity_sun = 0.7f;

	// torch
	m_data.pos_torch = { 0.f, 0.f, 0.f };
	m_data.color_torch = { 1.f, 0.7f, 0.2f };
	m_data.intensity_torch = 1.2f;
	m_data.attConst = 0.5f;
	m_data.attLin = 0.15f;
	m_data.attQuad = 0.05f;

	m_pMainCamera = gfx.GetCamera();
}


void PointLight::Bind(Graphics& gfx, DirectX::XMMATRIX view) const
{
	// transform positions into view co-ordinates for shader
	auto dataCopy = m_data;
	auto pos_sun = DirectX::XMLoadFloat3(&m_data.pos_sun);
	auto pos_torch = DirectX::XMLoadFloat3(&m_pMainCamera->GetPos());
	DirectX::XMStoreFloat3(&dataCopy.pos_sun, DirectX::XMVector3Transform(pos_sun, view));
	DirectX::XMStoreFloat3(&dataCopy.pos_torch, DirectX::XMVector3Transform(pos_torch, view));
	m_lightBuffer.Update(gfx, dataCopy);
	m_lightBuffer.Bind(gfx);

	LightMatrixBuffer lmb =
	{
		DirectX::XMMatrixTranspose(m_camera.GetView()),
		DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicLH(
			3.f, 3.f, -m_data.pos_sun.z - 2.f, -m_data.pos_sun.z + 20.f))
	};
	m_matrixBuffer.Update(gfx, lmb);
	m_matrixBuffer.Bind(gfx);
}

