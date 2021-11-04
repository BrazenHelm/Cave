#include "IndexBuffer.h"


IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices) :
	m_count((UINT)indices.size())
{
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.ByteWidth = UINT(m_count * sizeof(unsigned short));
	bd.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = indices.data();

	GetDevice(gfx)->CreateBuffer(&bd, &sd, &m_pBuf);
}


void IndexBuffer::Bind(Graphics& gfx)
{
	GetContext(gfx)->IASetIndexBuffer(m_pBuf.Get(), DXGI_FORMAT_R16_UINT, 0);
}
