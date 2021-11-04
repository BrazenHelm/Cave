#include "GodRay.h"
#include "Bindables.h"
#include "Material.h"


GodRay::GodRay(Graphics& gfx)
{
	CreateMesh(4.f, 4.f, 20.f, 0.1f);
	AddStaticMesh(gfx, m_vertices, m_indices);
	LoadStaticTexture(gfx, "Assets/plain_white.png");
	AddBind(std::make_unique<MatrixBuffer>(gfx, *this, 0));

	Material mat({ 1.f, 1.f, 1.f }, 1.f, 100.f);
	AddBind(std::make_unique<PixelConstantBuffer<Material>>(gfx, mat, 1));

	transform.SetPos(8.f, 8.f, 0.f);
	transform.SetRot(DirectX::XM_PI / 6.f, 0.f, 0.f);
	transform.SetScale(1.f, 1.f, 1.f);
}

// Makes a cuboid-shaped (width x height x depth) mesh of cubes (side length fineness)
void GodRay::CreateMesh(float width, float height, float depth, float fineness)
{
	m_vertices = std::vector<objl::Vertex>();
	m_indices = std::vector<unsigned short>();

	int nx = static_cast<int>(roundf(width / fineness));
	int ny = static_cast<int>(roundf(height / fineness));
	int nz = static_cast<int>(roundf(depth / fineness / 10.f));

	for (int x = 0; x < nx; x++)
	{
		for (int y = 0; y < ny; y++)
		{
			AddCubeMesh(x * fineness, y * fineness, depth, fineness);
			/*for (int z = 0; z < nz; z++)
			{
				AddCubeMesh(x * fineness, y * fineness, z * fineness * 10.f, fineness);
			}*/
		}
	}
}


void GodRay::AddCubeMesh(float x, float y, float zSize, float size)
{
	using namespace objl;
	
	unsigned short index = m_vertices.size();

	std::vector<Vertex> verts;
	verts.resize(8);
	verts[0].Position = Vector3(x, y, 0);
	verts[1].Position = Vector3(x, y, zSize);
	verts[2].Position = Vector3(x, y + size, 0);
	verts[3].Position = Vector3(x, y + size, zSize);
	verts[4].Position = Vector3(x + size, y, 0);
	verts[5].Position = Vector3(x + size, y, zSize);
	verts[6].Position = Vector3(x + size, y + size, 0);
	verts[7].Position = Vector3(x + size, y + size, zSize);

	m_vertices.insert(m_vertices.end(), verts.begin(), verts.end());

	unsigned short inds[36] = {
		index + 0, index + 2, index + 4,
		index + 2, index + 6, index + 4,

		index + 1, index + 5, index + 3,
		index + 3, index + 5, index + 7,

		index + 0, index + 4, index + 1,
		index + 1, index + 4, index + 5,

		index + 2, index + 3, index + 6,
		index + 3, index + 7, index + 6,

		index + 0, index + 1, index + 2,
		index + 1, index + 3, index + 2,

		index + 4, index + 6, index + 5,
		index + 5, index + 6, index + 7
	};

	m_indices.insert(m_indices.end(), inds, inds + 36);
}

