#include "PrimitiveTopology.h"



PrimitiveTopology::PrimitiveTopology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type) :
	m_type(type)
{}


void PrimitiveTopology::Bind(Graphics& gfx)
{
	GetContext(gfx)->IASetPrimitiveTopology(m_type);
}
