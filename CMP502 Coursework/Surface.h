#pragma once
#include "WindowsInclude.h"
#include "Color.h"
#include <string>
#include <assert.h>
#include <memory>


class Surface
{
public:
	Surface(unsigned int width, unsigned int height);
	Surface(Surface&& source);
	Surface(Surface&) = delete;
	Surface& operator=(Surface&& donor);
	Surface& operator=(const Surface&) = delete;
	~Surface();

	void Clear(Color fill);
	void PutPixel(unsigned int x, unsigned int y, Color c);
	Color GetPixel(unsigned int x, unsigned int y) const;
	unsigned int GetWidth() const { return m_width; }
	unsigned int GetHeight() const { return m_height; }
	Color* GetBufferPtr() { return m_pBuffer.get(); }
	const Color* GetBufferPtr() const { return m_pBuffer.get(); }
	const Color* GetBufferPtrConst() const { return m_pBuffer.get(); }

	static Surface FromFile(const std::string& path);
	void Save(const std::string& path) const;
	void Copy(const Surface& source);

private:
	Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBuf);

private:
	std::unique_ptr<Color[]> m_pBuffer;
	unsigned int m_width, m_height;
};

