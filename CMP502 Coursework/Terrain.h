#pragma once
#include "DrawableBase.h"
#include "MarchingCubesGrid.h"
#include "Material.h"

class Terrain : public DrawableBase<Terrain>
{
private:
	MarchingCubesGrid m_grid;

public:
	Terrain(Graphics& gfx, const Material& material);

	void Mine(Graphics& gfx, objl::Vector3 pos, objl::Vector3 dir);
	bool Collision(float x, float y, float z, float r, objl::Vector3& vectorOut);

private:
	bool Collision(objl::Vector3 A, objl::Vector3 B, objl::Vector3 C,
		objl::Vector3 N, objl::Vector3 P, float r, float& d);
	bool Collision(objl::Vector3 A, objl::Vector3 B,
		objl::Vector3 P, float r, float& d);
	bool SameSide(objl::Vector3 A, objl::Vector3 B, objl::Vector3 C, objl::Vector3 Q);
};

