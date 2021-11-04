#pragma once
#include <DirectXMath.h>


struct Material
{
	Material() :
		color(1.f, 1.f, 1.f), specularIntensity(0.8f), specularExponent(30.f)
	{};

	Material(DirectX::XMFLOAT3 c, float si, float se) :
		color(c), specularIntensity(si), specularExponent(se)
	{};

	alignas(16) DirectX::XMFLOAT3 color;
	float specularIntensity;
	float specularExponent;
	float padding[2];
};
