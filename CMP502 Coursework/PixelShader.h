#pragma once
#include "Bindable.h"
#include <string>

class PixelShader : public Bindable
{
public:
	PixelShader(Graphics& gfx, const std::string& path);
	void Bind(Graphics& gfx) override;

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pShader;
	Microsoft::WRL::ComPtr<ID3DBlob> m_pBlob;
	std::string m_path;
};

