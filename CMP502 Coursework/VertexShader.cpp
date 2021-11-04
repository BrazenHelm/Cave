#include "VertexShader.h"

#include <d3dcompiler.h>


VertexShader::VertexShader(Graphics& gfx, const std::string& path) :
	m_path(path)
{
	std::wstring stemp = std::wstring(path.begin(), path.end());
	D3DReadFileToBlob(stemp.c_str(), &m_pBlob);
	GetDevice(gfx)->CreateVertexShader(
		m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pShader
	);
}

//VertexShader::VertexShader(Graphics& gfx)
//{
//	D3DReadFileToBlob(L"basic_vs.cso", &m_pBlob);
//	GetDevice(gfx)->CreateVertexShader(
//		m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pShader
//	);
//}


void VertexShader::Bind(Graphics& gfx)
{
	GetContext(gfx)->VSSetShader(m_pShader.Get(), 0, 0);
}

