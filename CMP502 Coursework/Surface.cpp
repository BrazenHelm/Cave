#define FULL_WINTARD
#include "Surface.h"
#include <algorithm>
namespace GdiPlus
{
	using std::min;
	using std::max;
}
#include <gdiplus.h>
#include <sstream>

#pragma comment(lib, "gdiplus.lib")


Surface::Surface(unsigned int width, unsigned int height) :
	m_pBuffer(std::make_unique<Color[]>(width * height)),
	m_width(width), m_height(height)
{}


Surface& Surface::operator=(Surface&& donor)
{
	m_width = donor.m_width;
	m_height = donor.m_height;
	m_pBuffer = std::move(donor.m_pBuffer);
	donor.m_pBuffer = nullptr;
	return *this;
}


Surface::Surface(Surface&& source) :
	m_pBuffer(std::move(source.m_pBuffer)),
	m_width(source.m_width), m_height(source.m_height)
{}


Surface::~Surface() {}


void Surface::Clear(Color fill)
{
	memset(m_pBuffer.get(), fill.dword, m_width * m_height * sizeof(Color));
}


void Surface::PutPixel(unsigned int x, unsigned int y, Color c)
{
	assert(x >= 0); assert(y >= 0); assert(x < m_width); assert(y < m_height);
	m_pBuffer[y * m_width + x] = c;
}


Color Surface::GetPixel(unsigned int x, unsigned int y) const
{
	assert(x >= 0); assert(y >= 0); assert(x < m_width); assert(y < m_height);
	return m_pBuffer[y * m_width + x];
}


Surface Surface::FromFile(const std::string& path)
{
	unsigned int width = 0;
	unsigned int height = 0;
	std::unique_ptr<Color[]> pBuffer = nullptr;

	{
		wchar_t widename[512];
		mbstowcs_s(nullptr, widename, path.c_str(), _TRUNCATE);

		Gdiplus::Bitmap bitmap(widename);
		assert(bitmap.GetLastStatus() == Gdiplus::Status::Ok);

		width = bitmap.GetWidth();
		height = bitmap.GetHeight();
		pBuffer = std::make_unique<Color[]>(width * height);

		for (unsigned int y = 0; y < height; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				Gdiplus::Color c;
				bitmap.GetPixel(x, y, &c);
				pBuffer[y * width + x] = c.GetValue();
			}
		}
	}

	return Surface(width, height, std::move(pBuffer));
}


void Surface::Save(const std::string& path) const
{
	auto GetEncoderClsid = [&path](const WCHAR* format, CLSID* pClsid) -> void
	{
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes

		Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;

		Gdiplus::GetImageEncodersSize(&num, &size);
		assert(size != 0);

		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
		assert(pImageCodecInfo != nullptr);

		GetImageEncoders(num, size, pImageCodecInfo);

		for (UINT j = 0; j < num; ++j)
		{
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return;
			}
		}

		free(pImageCodecInfo);
	};

	CLSID bmpID;
	GetEncoderClsid(L"image/bmp", &bmpID);

	// convert filenam to wide string (for Gdiplus)
	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, path.c_str(), _TRUNCATE);

	Gdiplus::Bitmap bitmap(m_width, m_height, m_width * sizeof(Color),
		PixelFormat32bppARGB, (BYTE*)m_pBuffer.get());
	assert(bitmap.Save(wideName, &bmpID, nullptr) == Gdiplus::Status::Ok);
}


void Surface::Copy(const Surface& source)
{
	assert(m_width == source.m_width); assert(m_height == source.m_height);
	memcpy(m_pBuffer.get(), source.m_pBuffer.get(),
		m_width * m_height * sizeof(Color));
}


Surface::Surface(unsigned int width, unsigned int height,
	std::unique_ptr<Color[]> pBuffer) :
	m_pBuffer(std::move(pBuffer)),
	m_width(width), m_height(height)
{}

