#pragma once
#include "Bindable.h"

class PrimitiveTopology : public Bindable
{
public:
	PrimitiveTopology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type);
	void Bind(Graphics& gfx) override;

private:
	D3D11_PRIMITIVE_TOPOLOGY m_type;
};

