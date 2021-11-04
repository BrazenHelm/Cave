#pragma once

#include "Bindable.h"

#include <vector>

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices);
	void Bind(Graphics& gfx) override;
	UINT GetCount() const { return m_count; }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuf;
	UINT m_count;
};

