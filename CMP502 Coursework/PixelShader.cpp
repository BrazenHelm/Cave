#include "PixelShader.h"

#include <d3dcompiler.h>

PixelShader::PixelShader(Graphics& gfx, const std::string& path) :
	m_path(path)
{
	std::wstring stemp = std::wstring(path.begin(), path.end());
	D3DReadFileToBlob(stemp.c_str(), &m_pBlob);
	GetDevice(gfx)->CreatePixelShader(
		m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pShader
	);
}


void PixelShader::Bind(Graphics& gfx)
{
	GetContext(gfx)->PSSetShader(m_pShader.Get(), 0, 0);
}

