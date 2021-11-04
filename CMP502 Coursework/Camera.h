#pragma once
#include <DirectXMath.h>
#include "Inputs.h"

class Camera
{
public:
	Camera();
	~Camera() = default;

	DirectX::XMMATRIX GetView() const;

	void SetPosition(float x, float y, float z);
	void LookAt(float x, float y, float z);
	void SetPitchYaw(float pitch, float yaw);

	void Control(Inputs inputs, float dt);

	DirectX::XMFLOAT3 GetPos() { return m_pos; }
	DirectX::XMFLOAT3 GetDir() { return m_forward; }

private:
	void Update();

private:
	DirectX::XMFLOAT3 m_pos;
	DirectX::XMFLOAT3 m_focus;
	DirectX::XMFLOAT3 m_up;

	DirectX::XMFLOAT3 m_forward;
	DirectX::XMFLOAT3 m_right;

	DirectX::XMMATRIX m_view;

	float m_pitch;	// in deg: 0 = level, +90 = up, -90 = down
	float m_yaw;	// in deg: 0 = x-dir, 90 = z-dir
	const float DEG_TO_RAD = DirectX::XM_PI / 180.f;
};

