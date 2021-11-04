#pragma once
#include "Bindable.h"
#include <vector>
#include <string>

class VertexBuffer : public Bindable
{
public:
	template<class V>
	VertexBuffer(Graphics& gfx, const std::vector<V>& vertices) :
		m_stride(sizeof(V))
	{
		D3D11_BUFFER_DESC vbd = {};
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.Usage = D3D11_USAGE_DEFAULT;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.ByteWidth = UINT(sizeof(V) * vertices.size());
		vbd.StructureByteStride = sizeof(V);

		D3D11_SUBRESOURCE_DATA vsd = {};
		vsd.pSysMem = vertices.data();

		GetDevice(gfx)->CreateBuffer(&vbd, &vsd, &m_pBuf);
	}

	void Bind(Graphics& gfx) override;
	
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuf;
	const UINT m_stride;
};

