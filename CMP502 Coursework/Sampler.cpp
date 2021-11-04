#include "Sampler.h"


Sampler::Sampler(Graphics& gfx)
{
	D3D11_SAMPLER_DESC sd = {};
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;

	GetDevice(gfx)->CreateSamplerState(&sd, &m_pSampler);
}


void Sampler::Bind(Graphics& gfx)
{
	GetContext(gfx)->PSSetSamplers(0, 1, m_pSampler.GetAddressOf());
}

