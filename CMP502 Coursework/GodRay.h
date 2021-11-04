#pragma once
#include "DrawableBase.h"
#include <vector>
#include "OBJ_Loader.h"

class GodRay : public DrawableBase<GodRay>
{
public:
	GodRay(Graphics& gfx);

private:
	std::vector<objl::Vertex> m_vertices;
	std::vector<unsigned short> m_indices;

	void CreateMesh(float width, float height, float depth, float fineness);
	void AddCubeMesh(float x, float y, float z, float size);
};

