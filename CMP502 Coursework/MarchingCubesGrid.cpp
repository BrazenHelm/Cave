#include "MarchingCubesGrid.h"

#include <queue>
#include <random>


MarchingCubesGrid::MarchingCubesGrid(int height, int width, int depth) :
	m_height(height), m_width(width), m_depth(depth)
{
	m_rng.seed(std::random_device{}());
	m_dist = std::uniform_real_distribution<double>(0.0, 1.0);

	m_fieldValues.resize(m_width);
	m_cellMap.resize(m_width);
	m_meshVertices.resize(m_width);
	m_meshIndices.resize(m_width);

	for (int x = 0; x < m_width; x++)
	{
		m_fieldValues[x].resize(m_height);
		m_cellMap[x].resize(m_height);
		m_meshVertices[x].resize(m_height);
		m_meshIndices[x].resize(m_height);
		for (int y = 0; y < m_height; y++)
		{
			m_fieldValues[x][y].resize(m_depth);
			m_cellMap[x][y].resize(m_depth);
			m_meshVertices[x][y].resize(m_depth);
			m_meshIndices[x][y].resize(m_depth);
			for (int z = 0; z < m_depth; z++)
			{
				m_fieldValues[x][y][z] = 0.0;
				m_cellMap[x][y][z] = false;
				m_meshVertices[x][y][z] = std::vector<objl::Vertex>();
				m_meshIndices[x][y][z] = std::vector<unsigned short>();
			}
		}
	}

	SeedCellMap(0.45);

	double neighbourWeightSum =
		4.0 * (W_SIDE + W_DIAG + W_ABOVE_SIDE + W_ABOVE_DIAG + W_BELOW_SIDE + W_BELOW_DIAG)
		+ W_ABOVE + W_BELOW;

	int nSimSteps = 5;
	for (int i = 0; i < nSimSteps; i++)
	{
		double birthThreshold = (14.0 + i) * neighbourWeightSum / 26.0;
		double deathThreshold = (12.0 - i) * neighbourWeightSum / 26.0;
		DoSimulationStep(birthThreshold, deathThreshold);
	}

	DrillHole();

	RemoveFloatingRock();

	for (int x = 0; x < m_width; x++)
	{
		for (int y = 0; y < m_height; y++)
		{
			for (int z = 0; z < m_depth; z++)
			{
				if (m_cellMap[x][y][z])
				{
					m_fieldValues[x][y][z] = m_dist(m_rng) * 0.4;
				}
				else
				{
					m_fieldValues[x][y][z] = 0.6 + m_dist(m_rng) * 0.4;
				}
			}
		}
	}

	CreateMesh(0.5);
}


void MarchingCubesGrid::SeedCellMap(double seedChance)
{
	for (int x = 0; x < m_width; x++)
	{
		for (int y = 0; y < m_height; y++)
		{
			for (int z = 0; z < m_depth; z++)
			{
				if (x == 0 || y == 0 || z == 0 || x == m_width - 1 ||
					y == m_height - 1 || z == m_depth - 1)
				{
					m_cellMap[x][y][z] = false;
				}
				else if (x == 1 || y == 1 || z == 1 || x == m_width - 2 ||
					y == m_height - 2 || z == m_depth - 2)
				{
					m_cellMap[x][y][z] = true;
				}
				else if (m_dist(m_rng) < seedChance)
				{
					m_cellMap[x][y][z] = true;
				}
				else
				{
					m_cellMap[x][y][z] = false;
				}
			}
		}
	}
}


void MarchingCubesGrid::DoSimulationStep(double birthThreshold, double deathThreshold)
{
	std::vector<std::vector<std::vector<bool>>> newCellMap = m_cellMap;

	for (int x = 2; x < m_width - 2; x++)
	{
		for (int y = 2; y < m_height - 2; y++)
		{
			for (int z = 2; z < m_depth - 2; z++)
			{
				double neighboursAlive = CountNeighboursAlive(x, y, z);

				if (m_cellMap[x][y][z])
				{
					if (neighboursAlive < deathThreshold)
						newCellMap[x][y][z] = false;
					else
						newCellMap[x][y][z] = true;
				}
				else
				{
					if (neighboursAlive > birthThreshold)
						newCellMap[x][y][z] = true;
					else
						newCellMap[x][y][z] = false;
				}
			}
		}
	}

	m_cellMap = newCellMap;
}


double MarchingCubesGrid::CountNeighboursAlive(int x, int y, int z)
{
	double neighboursAlive = 0.0;

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			for (int k = -1; k <= 1; k++)
			{
				int nx = x + i;
				int ny = y + j;
				int nz = z + k;

				if (m_cellMap[nx][ny][nz])
				{
					neighboursAlive += GetNeighbourWeight(i, j, k);
				}				
			}
		}
	}

	return neighboursAlive;
}


double MarchingCubesGrid::GetNeighbourWeight(int i, int j, int k)
{
	if (j == -1)
	{
		if (i != 0 && k != 0)
			return W_BELOW_DIAG;
		else if (i != 0 || k != 0)
			return W_BELOW_SIDE;
		else
			return W_BELOW;
	}
	else if (j == 0)
	{
		if (i != 0 && k != 0)
			return W_DIAG;
		else if (i != 0 || k != 0)
			return W_SIDE;
		else
			return W_SELF;
	}
	else if (j == 1)
	{
		if (i != 0 && k != 0)
			return W_ABOVE_DIAG;
		else if (i != 0 || k != 0)
			return W_ABOVE_SIDE;
		else
			return W_ABOVE;
	}
}


void MarchingCubesGrid::DrillHole()
{
	for (int x = 9; x < 11; x++)
	{
		for (int y = 9; y < 11; y++)
		{
			int z = 1;
			while (m_cellMap[x][y][z])
			{
				m_cellMap[x][y][z] = false;
				z++;
			}
		}
	}
}


void MarchingCubesGrid::RemoveFloatingRock()
{
	std::vector<std::vector<std::vector<bool>>> isConnected;
	isConnected.resize(m_width);

	for (int x = 0; x < m_width; x++)
	{
		isConnected[x].resize(m_height);
		for (int y = 0; y < m_height; y++)
		{
			isConnected[x][y].resize(m_depth);
			for (int z = 0; z < m_depth; z++)
			{
				isConnected[x][y][z] = false;
			}
		}
	}

	struct LatticePoint
	{
		int x, y, z;
		LatticePoint(int x, int y, int z) : x(x), y(y), z(z) {};
	};
	std::queue<LatticePoint> fillQueue;
	fillQueue.push(LatticePoint(1, 1, 1));

	while (!fillQueue.empty())
	{
		LatticePoint p = fillQueue.front();
		fillQueue.pop();

		if (!m_cellMap[p.x][p.y][p.z]) continue;
		if (isConnected[p.x][p.y][p.z]) continue;

		isConnected[p.x][p.y][p.z] = true;

		fillQueue.push(LatticePoint(p.x + 1, p.y, p.z));
		fillQueue.push(LatticePoint(p.x - 1, p.y, p.z));
		fillQueue.push(LatticePoint(p.x, p.y + 1, p.z));
		fillQueue.push(LatticePoint(p.x, p.y - 1, p.z));
		fillQueue.push(LatticePoint(p.x, p.y, p.z + 1));
		fillQueue.push(LatticePoint(p.x, p.y, p.z - 1));
	}

	for (int x = 0; x < m_width; x++)
	{
		for (int y = 0; y < m_height; y++)
		{
			for (int z = 0; z < m_depth; z++)
			{
				if (m_cellMap[x][y][z] && !isConnected[x][y][z])
				{
					m_cellMap[x][y][z] = false;
				}
			}
		}
	}
}


void MarchingCubesGrid::CreateMesh(double isolevel,
	std::vector<objl::Vertex>& vertices, std::vector<unsigned short>& indices)
{
	for (int x = 0; x < m_width - 1; x++)
	{
		for (int y = 0; y < m_height - 1; y++)
		{
			for (int z = 0; z < m_depth - 1; z++)
			{
				GridCell cell = GetCell(x, y, z);
				AddMesh(cell, isolevel, vertices, indices);
			}
		}
	}
}


void MarchingCubesGrid::CreateMesh(double isolevel)
{
	for (int x = 0; x < m_width - 1; x++)
	{
		for (int y = 0; y < m_height - 1; y++)
		{
			for (int z = 0; z < m_depth - 1; z++)
			{
				CreateCellMesh(x, y, z, isolevel);
			}
		}
	}
}


void MarchingCubesGrid::AddMesh(GridCell cell, double isolevel,
	std::vector<objl::Vertex>& vertices, std::vector<unsigned short>& indices)
{
	using namespace objl;
	using namespace objl::math;

	int index = 0;
	Vector3 vertexPosList[12];

	if (cell.values[0] < isolevel) index |= 1;
	if (cell.values[1] < isolevel) index |= 2;
	if (cell.values[2] < isolevel) index |= 4;
	if (cell.values[3] < isolevel) index |= 8;
	if (cell.values[4] < isolevel) index |= 16;
	if (cell.values[5] < isolevel) index |= 32;
	if (cell.values[6] < isolevel) index |= 64;
	if (cell.values[7] < isolevel) index |= 128;

	int edges = EDGE_TABLE[index];

	// cell is entirely inside or entirely outside surface
	if (edges == 0) return;

	// find positions of vertices where the isosurface insersects the cube edges
	if (edges & 1) vertexPosList[0] = Lerp(
		isolevel, cell.corners[0], cell.corners[1], cell.values[0], cell.values[1]);
	if (edges & 2) vertexPosList[1] = Lerp(
		isolevel, cell.corners[1], cell.corners[2], cell.values[1], cell.values[2]);
	if (edges & 4) vertexPosList[2] = Lerp(
		isolevel, cell.corners[2], cell.corners[3], cell.values[2], cell.values[3]);
	if (edges & 8) vertexPosList[3] = Lerp(
		isolevel, cell.corners[3], cell.corners[0], cell.values[3], cell.values[0]);
	if (edges & 16) vertexPosList[4] = Lerp(
		isolevel, cell.corners[4], cell.corners[5], cell.values[4], cell.values[5]);
	if (edges & 32) vertexPosList[5] = Lerp(
		isolevel, cell.corners[5], cell.corners[6], cell.values[5], cell.values[6]);
	if (edges & 64) vertexPosList[6] = Lerp(
		isolevel, cell.corners[6], cell.corners[7], cell.values[6], cell.values[7]);
	if (edges & 128) vertexPosList[7] = Lerp(
		isolevel, cell.corners[7], cell.corners[4], cell.values[7], cell.values[4]);
	if (edges & 256) vertexPosList[8] = Lerp(
		isolevel, cell.corners[0], cell.corners[4], cell.values[0], cell.values[4]);
	if (edges & 512) vertexPosList[9] = Lerp(
		isolevel, cell.corners[1], cell.corners[5], cell.values[1], cell.values[5]);
	if (edges & 1024) vertexPosList[10] = Lerp(
		isolevel, cell.corners[2], cell.corners[6], cell.values[2], cell.values[6]);
	if (edges & 2048) vertexPosList[11] = Lerp(
		isolevel, cell.corners[3], cell.corners[7], cell.values[3], cell.values[7]);

	// add on triangles
	for (int i = 0; TRI_TABLE[index][i] != -1; i += 3)
	{
		Vertex v0, v1, v2;
		v0.Position = vertexPosList[TRI_TABLE[index][i]];
		v1.Position = vertexPosList[TRI_TABLE[index][i + 1]];
		v2.Position = vertexPosList[TRI_TABLE[index][i + 2]];

		Vector3 n = CrossV3((v1.Position - v0.Position), (v2.Position - v0.Position));
		n = n / MagnitudeV3(n);
		v0.Normal = v1.Normal = v2.Normal = n;

		unsigned short firstIndex = vertices.size();
		vertices.emplace_back(v0);
		vertices.emplace_back(v1);
		vertices.emplace_back(v2);
		indices.emplace_back(firstIndex);
		indices.emplace_back(firstIndex + 1);
		indices.emplace_back(firstIndex + 2);
	}
};


void MarchingCubesGrid::CreateCellMesh(int x, int y, int z, double isolevel)
{
	m_meshVertices[x][y][z].clear();
	m_meshIndices[x][y][z].clear();

	//GridCell cell = GetCell(x, y, z);

	AddMesh(GetCell(x, y, z), isolevel, m_meshVertices[x][y][z], m_meshIndices[x][y][z]);
}


void MarchingCubesGrid::GetMesh(std::vector<objl::Vertex>& vertices, std::vector<unsigned short>& indices)
{
	vertices.clear();
	indices.clear();

	for (int x = 0; x < m_width; x++)
	{
		for (int y = 0; y < m_height; y++)
		{
			for (int z = 0; z < m_depth; z++)
			{
				unsigned short firstIndex = vertices.size();
				for (int i = 0; i < m_meshVertices[x][y][z].size(); i++)
				{
					vertices.push_back(m_meshVertices[x][y][z][i]);
				}
				for (int j = 0; j < m_meshIndices[x][y][z].size(); j++)
				{
					indices.push_back(m_meshIndices[x][y][z][j] + firstIndex);
				}
			}
		}
	}
}


void MarchingCubesGrid::GetCellMesh(int x, int y, int z,
	std::vector<objl::Vertex>& vertices, std::vector<unsigned short>& indices)
{
	if (x < 0 || y < 0 || z < 0 || x > m_width - 2 || y > m_height - 2 || z > m_depth - 2)
		return;

	vertices = m_meshVertices[x][y][z];
	indices = m_meshIndices[x][y][z];
}


void MarchingCubesGrid::Mine(int x, int y, int z)
{
	if (m_cellMap[x][y][z] == false) return;

	m_cellMap[x][y][z] = false;
	m_fieldValues[x][y][z] = 0.6 + 0.4 * m_dist(m_rng);

	CreateCellMesh(x - 1, y - 1, z - 1, 0.5);
	CreateCellMesh(x - 1, y - 1, z, 0.5);
	CreateCellMesh(x - 1, y, z - 1, 0.5);
	CreateCellMesh(x - 1, y, z, 0.5);
	CreateCellMesh(x, y - 1, z - 1, 0.5);
	CreateCellMesh(x, y - 1, z, 0.5);
	CreateCellMesh(x, y, z - 1, 0.5);
	CreateCellMesh(x, y, z, 0.5);
}


MarchingCubesGrid::GridCell MarchingCubesGrid::GetCell(int x, int y, int z)
{
	if (x < 0 || y < 0 || z < 0 || x > m_width - 2 || y > m_height - 2 || z > m_depth - 2)
	{
		return GridCell();
	}

	GridCell cell;

	cell.corners[0] = objl::Vector3(x, y, z);
	cell.corners[1] = objl::Vector3(x + 1, y, z);
	cell.corners[2] = objl::Vector3(x + 1, y, z + 1);
	cell.corners[3] = objl::Vector3(x, y, z + 1);
	cell.corners[4] = objl::Vector3(x, y + 1, z);
	cell.corners[5] = objl::Vector3(x + 1, y + 1, z);
	cell.corners[6] = objl::Vector3(x + 1, y + 1, z + 1);
	cell.corners[7] = objl::Vector3(x, y + 1, z + 1);

	cell.values[0] = m_fieldValues[x][y][z];
	cell.values[1] = m_fieldValues[x + 1][y][z];
	cell.values[2] = m_fieldValues[x + 1][y][z + 1];
	cell.values[3] = m_fieldValues[x][y][z + 1];
	cell.values[4] = m_fieldValues[x][y + 1][z];
	cell.values[5] = m_fieldValues[x + 1][y + 1][z];
	cell.values[6] = m_fieldValues[x + 1][y + 1][z + 1];
	cell.values[7] = m_fieldValues[x][y + 1][z + 1];

	return cell;
}


objl::Vector3 MarchingCubesGrid::Lerp(double isolevel,
	objl::Vector3 pos1, objl::Vector3 pos2, double val1, double val2)
{
	double eps = 0.00001;
	if (std::abs(isolevel - val1) < eps) return pos1;
	if (std::abs(isolevel - val2) < eps) return pos2;
	if (std::abs(val1 - val2) < eps) return pos1;

	double t = (isolevel - val1) / (val2 - val1);
	return objl::Vector3(
		(1.0 - t) * pos1.X + t * pos2.X,
		(1.0 - t) * pos1.Y + t * pos2.Y,
		(1.0 - t) * pos1.Z + t * pos2.Z
	);
}

