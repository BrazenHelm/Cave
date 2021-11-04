#include "MatrixBuffer.h"


std::unique_ptr<VertexConstantBuffer<MatrixBuffer::Transforms>> MatrixBuffer::m_pVCBuf;


MatrixBuffer::MatrixBuffer(Graphics& gfx, const Drawable& parent, UINT slot) :
	m_parent(parent)
{
	if (m_pVCBuf == nullptr)
	{
		m_pVCBuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
	}
}


void MatrixBuffer::Bind(Graphics& gfx)
{
	const DirectX::XMMATRIX cam = m_parent.GetTransformXM() * gfx.GetCameraMatrix();
	const Transforms tf =
	{
		DirectX::XMMatrixTranspose(m_parent.GetTransformXM()),
		DirectX::XMMatrixTranspose(gfx.GetCameraMatrix()),
		DirectX::XMMatrixTranspose(gfx.GetProjection())
	};

	m_pVCBuf->Update(gfx, tf);
	m_pVCBuf->Bind(gfx);
}

