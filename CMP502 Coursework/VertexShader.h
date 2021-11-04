#pragma once
#include "Bindable.h"
#include <string>

class VertexShader : public Bindable
{
public:
	VertexShader(Graphics& gfx, const std::string& path);
	void Bind(Graphics& gfx) override;

	ID3DBlob* GetBytecode() const { return m_pBlob.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pShader;
	Microsoft::WRL::ComPtr<ID3DBlob> m_pBlob;
	std::string m_path;
};

