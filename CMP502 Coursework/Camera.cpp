#include "Camera.h"

namespace dx = DirectX;

Camera::Camera() :
	m_up(0.f, 1.f, 0.f), m_pitch(0.f), m_yaw(0.f)
{}


dx::XMMATRIX Camera::GetView() const
{
	return m_view;
}


void Camera::SetPosition(float x, float y, float z)
{
	m_pos = { x, y, z };
	Update();
}


void Camera::LookAt(float x, float y, float z)
{
	m_focus = { x, y, z };
	dx::XMStoreFloat3(&m_forward, dx::XMVector3Normalize(dx::XMVectorSet(x - m_pos.x, y - m_pos.y, z - m_pos.z, 0.f)));
	m_pitch = asinf(m_forward.y) / DEG_TO_RAD;
	m_yaw = -atan2f(m_forward.z, m_forward.x) / DEG_TO_RAD;
	Update();
}


void Camera::SetPitchYaw(float pitch, float yaw)
{
	m_pitch = pitch;
	m_yaw = yaw;
	Update();
}


void Camera::Control(Inputs inputs, float dt)
{
	const float MOVE_SPEED = 3.f;
	const float TURN_SPEED = 0.35f;

	float dx = m_forward.x * inputs.moveForward + m_right.x * inputs.moveRight + m_up.x * inputs.moveUp;
	float dy = m_forward.y * inputs.moveForward + m_right.y * inputs.moveRight + m_up.y * inputs.moveUp;
	float dz = m_forward.z * inputs.moveForward + m_right.z * inputs.moveRight + m_up.z * inputs.moveUp;

	m_pos.x += dx * MOVE_SPEED * dt;
	m_pos.y += dy * MOVE_SPEED * dt;
	m_pos.z += dz * MOVE_SPEED * dt;

	m_pitch += inputs.lookUp * TURN_SPEED;
	m_yaw += inputs.lookRight * TURN_SPEED;

	if (m_pitch > 80.f) m_pitch = 80.f;
	if (m_pitch < -80.f) m_pitch = -80.f;
	if (m_yaw > 180.f) m_yaw -= 360.f;
	if (m_yaw < 180.f) m_yaw += 360.f;


	//float dx = (m_forward.x * inputs.xPan + m_right.x * inputs.zPan) * dt * m_moveSpeed;
	//float dz = (m_forward.z * inputs.xPan + m_right.z * inputs.zPan) * dt * m_moveSpeed;
	//m_pos.x += dx;
	//m_pos.z += dz;
	//m_focus.x += dx;
	//m_focus.z += dz;

	//dx::XMMATRIX rotMatrix =
	//	dx::XMMatrixTranslation(-m_focus.x, -m_focus.y, -m_focus.z) *
	//	dx::XMMatrixRotationY(inputs.yawRot * dt * m_turnSpeed) *
	//	dx::XMMatrixTranslation(m_focus.x, m_focus.y, m_focus.z);
	//auto pos = dx::XMLoadFloat3(&m_pos);
	//dx::XMStoreFloat3(&m_pos, dx::XMVector3Transform(pos, rotMatrix));

	Update();
}


void Camera::Update()
{
	dx::XMVECTOR pos = dx::XMVectorSet(m_pos.x, m_pos.y, m_pos.z, 0.f);
	dx::XMVECTOR up = dx::XMVectorSet(m_up.x, m_up.y, m_up.z, 0.f);

	dx::XMVECTOR forward = dx::XMVectorSet(
		cosf(m_pitch * DEG_TO_RAD) * cosf(-m_yaw * DEG_TO_RAD),
		sinf(m_pitch * DEG_TO_RAD),
		cosf(m_pitch * DEG_TO_RAD) * sinf(-m_yaw * DEG_TO_RAD),
		0.f
	);

	m_view = dx::XMMatrixLookToLH(pos, forward, up);

	dx::XMStoreFloat3(&m_forward, forward);
	dx::XMStoreFloat3(&m_right, dx::XMVector3Normalize(dx::XMVector3Cross(up, forward)));

	//dx::XMVECTOR pos = dx::XMVectorSet(m_pos.x, m_pos.y, m_pos.z, 0.f);
	//dx::XMVECTOR focus = dx::XMVectorSet(m_focus.x, m_focus.y, m_focus.z, 0.f);
	//dx::XMVECTOR up = dx::XMVectorSet(m_up.x, m_up.y, m_up.z, 0.f);
	//m_view = dx::XMMatrixLookAtLH(pos, focus, up);

	//const dx::XMVECTOR forward = dx::XMVector4Normalize(
	//	dx::XMVectorSet(m_focus.x - m_pos.x, 0.f, m_focus.z - m_pos.z, 0.f));
	//dx::XMStoreFloat3(&m_forward, forward);

	//const dx::XMVECTOR right = dx::XMVector3Cross(up, forward);
	//dx::XMStoreFloat3(&m_right, right);
}

