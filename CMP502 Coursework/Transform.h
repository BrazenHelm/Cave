#pragma once
#include <DirectXMath.h>

class Transform
{
public:
	DirectX::XMMATRIX GetWorldMatrix() const;

	void SetPos(float x, float y, float z) { m_pos = { x, y, z }; }
	void SetRot(float x, float y, float z) { m_rot = { x, y, z }; }
	void SetScale(float x, float y, float z) { m_scale = { x, y, z }; }

private:
	DirectX::XMFLOAT3 m_pos;
	DirectX::XMFLOAT3 m_rot;
	DirectX::XMFLOAT3 m_scale;
};

