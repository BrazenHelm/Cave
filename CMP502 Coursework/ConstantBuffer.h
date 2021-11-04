#pragma once
#include "Bindable.h"

template<typename C>
class ConstantBuffer : public Bindable
{
public:
	void Update(Graphics& gfx, const C& data)
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		GetContext(gfx)->Map(m_pBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		memcpy(msr.pData, &data, sizeof(data));
		GetContext(gfx)->Unmap(m_pBuf.Get(), 0);
	}

	ConstantBuffer(Graphics& gfx, const C& data, UINT slot = 0u) :
		m_slot(slot)
	{
		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0;
		cbd.ByteWidth = sizeof(data);
		cbd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &data;

		GetDevice(gfx)->CreateBuffer(&cbd, &csd, &m_pBuf);
	}

	ConstantBuffer(Graphics& gfx, UINT slot = 0u) :
		m_slot(slot)
	{
		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0;
		cbd.ByteWidth = sizeof(C);
		cbd.StructureByteStride = 0;

		GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &m_pBuf);
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuf;
	UINT m_slot;
};


// Use a VertexConstantBuffer for constant buffers bound to the Vertex Shader
template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::m_pBuf;
	using ConstantBuffer<C>::m_slot;
	using Bindable::GetContext;

public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx) override
	{
		GetContext(gfx)->VSSetConstantBuffers(m_slot, 1, m_pBuf.GetAddressOf());
	}
};


// Use a PixelConstantBuffer for constant buffers bound to the Pixel Shader
template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::m_pBuf;
	using ConstantBuffer<C>::m_slot;
	using Bindable::GetContext;

public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx) override
	{
		GetContext(gfx)->PSSetConstantBuffers(m_slot, 1, m_pBuf.GetAddressOf());
	}
};