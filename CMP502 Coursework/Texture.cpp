#include "Texture.h"

namespace wrl = Microsoft::WRL;

Texture::Texture(Graphics& gfx, const Surface& s)
{
	// create texture resource
	D3D11_TEXTURE2D_DESC td = {};
	td.Width = s.GetWidth();
	td.Height = s.GetHeight();
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = s.GetBufferPtr();
	sd.SysMemPitch = s.GetWidth() * sizeof(Color);

	wrl::ComPtr<ID3D11Texture2D> pTexture;
	GetDevice(gfx)->CreateTexture2D(&td, &sd, &pTexture);

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
	srvd.Format = td.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;

	GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvd, &m_pTexView);
}


void Texture::Bind(Graphics& gfx)
{
	GetContext(gfx)->PSSetShaderResources(0, 1, m_pTexView.GetAddressOf());
}

