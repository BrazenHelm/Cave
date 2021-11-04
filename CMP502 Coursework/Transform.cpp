#include "Transform.h"


DirectX::XMMATRIX Transform::GetWorldMatrix() const
{
	return DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
		DirectX::XMMatrixRotationRollPitchYaw(m_rot.x, m_rot.y, m_rot.z) *
		DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	//return
	//	DirectX::XMMatrixRotationRollPitchYaw(m_rot.x, m_rot.y, m_rot.z) *
	//	DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
}

