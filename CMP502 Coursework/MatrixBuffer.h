#pragma once
#include "Bindable.h"
#include "ConstantBuffer.h"
#include "Drawable.h"
#include "DrawableBase.h"

#include <DirectXMath.h>

class MatrixBuffer : public Bindable
{
private:
	struct Transforms
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
	};

public:
	MatrixBuffer(Graphics& gfx, const Drawable& parent, UINT slot = 0u);
	void Bind(Graphics& gfx) override;

private:
	static std::unique_ptr<VertexConstantBuffer<Transforms>> m_pVCBuf;
	const Drawable& m_parent;
};

