#include "Terrain.h"
#include "Bindables.h"
#include "OBJ_Loader.h"


Terrain::Terrain(Graphics& gfx, const Material& material) :
	m_grid(20, 20, 20)
{
	std::vector<objl::Vertex> vertices;
	std::vector<unsigned short> indices;
	m_grid.GetMesh(vertices, indices);
	AddStaticMesh(gfx, vertices, indices);

	LoadStaticTexture(gfx, "Assets/plain_white.png");

	AddBind(std::make_unique<MatrixBuffer>(gfx, *this, 0));
	AddBind(std::make_unique<PixelConstantBuffer<Material>>(gfx, material, 1));

	//transform.SetPos(-5.f, -5.f, -5.f);
	transform.SetScale(1.f, 1.f, 1.f);
}


void Terrain::Mine(Graphics& gfx, objl::Vector3 pos, objl::Vector3 dir)
{
	int xr = static_cast<int>(roundf(pos.X));
	int yr = static_cast<int>(roundf(pos.Y));
	int zr = static_cast<int>(roundf(pos.Z));

	for (float t = 0.f; t < 20.f; t += 0.01f)
	{
		objl::Vector3 pt = pos + dir * t;
		int xr_new = static_cast<int>(roundf(pt.X));
		int yr_new = static_cast<int>(roundf(pt.Y));
		int zr_new = static_cast<int>(roundf(pt.Z));

		if (xr == xr_new && yr == yr_new && zr == zr_new)
		{
			continue;
		}

		xr = xr_new;
		yr = yr_new;
		zr = zr_new;

		if (m_grid.ContainsRock(xr, yr, zr))
		{
			m_grid.Mine(xr, yr, zr);
			std::vector<objl::Vertex> vertices;
			std::vector<unsigned short> indices;
			m_grid.GetMesh(vertices, indices);
			//AddStaticMesh(gfx, vertices, indices);
			// unfortunately, Drawables cannot have their meshes changed.
		}
	}
}


bool Terrain::Collision(float x, float y, float z, float r, objl::Vector3& vectorOut)
{
	// assumes r <= 0.5

	// nearest lattice vertex coordinates
	int xr = static_cast<int>(roundf(x));
	int yr = static_cast<int>(roundf(y));
	int zr = static_cast<int>(roundf(z));
	
	std::vector<objl::Vertex> vertices;
	std::vector<unsigned short> indices;

	objl::Vector3 P(x, y, z);		// centre of sphere

	float largestDistanceOut = 0.f;

	// we need to check the 8 adjacent grid cells
	for (int i = -1; i < 1; i++)
	{
		for (int j = -1; j < 1; j++)
		{
			for (int k = -1; k < 1; k++)
			{
				// Check for collision in this cell
				m_grid.GetCellMesh(xr + i, yr + j, zr + k, vertices, indices);
			
				// Check for collision with each triangle
				for (int triang = 0; triang < indices.size(); triang += 3)
				{
					objl::Vector3 N = vertices[triang].Normal;			// normal to triangle
					objl::Vector3 A = vertices[triang].Position;		// vertices of triangle
					objl::Vector3 B = vertices[triang + 1].Position;	// "
					objl::Vector3 C = vertices[triang + 2].Position;	// "

					float d;
					if (Collision(A, B, C, N, P, r, d))
					{
						if (d > largestDistanceOut)
						{
							largestDistanceOut = d;
							vectorOut = N * d;
						}
					}
				}		
			}
		}
	}

	if (largestDistanceOut > 0.f)
	{
		return true;
	}

	return false;
}


// * Sphere-Triangle Collision
// * Triangle vertices A, B, C, normal N. Sphere centre P, radius r.
// * d becomes the distance to move out along N to no longer be in the triangle
bool Terrain::Collision(objl::Vector3 A, objl::Vector3 B, objl::Vector3 C,
	objl::Vector3 N, objl::Vector3 P, float r, float& d)
{
	using namespace objl;
	using namespace objl::math;

	float k = DotV3(A, N);		// plane of triangle is defined by P.N = k
	float q = DotV3(P, N) - k;	// distance from centre of sphere to plane

	if (q > r) return false;	// sphere does not intersect plane
	if (q < 0) return false;	// centre of sphere is behind triangle

	Vector3 Q = P - N * q;		// normal projection of P onto plane

	if (SameSide(A, B, C, Q) && SameSide(B, C, A, Q) && SameSide(C, A, B, Q))
	{
		// Q is within ABC
		d = r - q;
		return true;
	}

	// Check for intersection between sphere and triangle edges
	float d1;
	if (Collision(A, B, P, r, d1))
	{
		// intersects with AB
		d = sqrtf(r * r + q * q - d1 * d1);
		return true;
	}
	if (Collision(B, C, P, r, d1))
	{
		// intersects with BC
		d = sqrtf(r * r + q * q - d1 * d1);
		return true;
	}
	if (Collision(C, A, P, r, d1))
	{
		// intersects with CA
		d = sqrtf(r * r + q * q - d1 * d1);
		return true;
	}

	return false;	// if we got here, sphere is off to the side of the triangle
}


// * Sphere-Line Collision
// * Line endpoints A, B. Sphere centre P, radius r
// * d becomes the distance from the centre to the line
bool Terrain::Collision(objl::Vector3 A, objl::Vector3 B,
	objl::Vector3 P, float r, float& d)
{
	using namespace objl;
	using namespace objl::math;

	// Q is the normal projection of P onto AB
	Vector3 Q = A + (B - A) * DotV3(P - A, B - A) / DotV3(B - A, B - A);

	if (MagnitudeV3(P - A) < r || MagnitudeV3(P - B) < r)
	{
		// A or B lies within sphere
		//d = MagnitudeV3(P - Q);
		return true;
	}

	if (MagnitudeV3(Q - A) < MagnitudeV3(B - A) &&
		MagnitudeV3(Q - B) < MagnitudeV3(A - B) &&
		MagnitudeV3(Q - P) < r)
	{
		// Q lies between A and B and is within sphere
		//d = MagnitudeV3(P - Q);
		return true;
	}

	return false;
}


// * Determines whether points C and Q are on the same side of AB or not
bool Terrain::SameSide(objl::Vector3 A, objl::Vector3 B, objl::Vector3 C, objl::Vector3 Q)
{
	auto cp1 = objl::math::CrossV3(B - A, C - A);
	auto cp2 = objl::math::CrossV3(B - A, Q - A);
	return objl::math::DotV3(cp1, cp2) > 0;
}

