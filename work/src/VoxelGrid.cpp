#include "VoxelGrid.hpp"
#include <iostream>

using namespace std;
using namespace glm;


VoxelGrid::VoxelGrid(glm::vec3 origin, glm::vec3 farEnd, float width) {
	
	m_width = width;
	m_countX = (farEnd.x - origin.x) / width;
	m_countY = (farEnd.y - origin.y) / width;
	m_countZ = (farEnd.z - origin.z) / width;

	m_grid = vector<vector<vector<Voxel>>> (m_countX, vector<vector<Voxel>> (m_countY, vector<Voxel> (m_countZ)));
	
	for (int i = 0; i < m_countX; i++) {
		for (int j = 0; j < m_countY; j++) {
			m_grid[i][j][0] = Voxel(glm::ivec3{i, j, 0}, TERRAIN, 10);
			m_grid[i][j][1] = Voxel(glm::ivec3{i, j, 0}, TALL_CELL, 5) ;

			for (int k = 2; k < 10; k++) {
				m_grid[i][j][k] = Voxel(glm::ivec3{i, j, k}, REG_CELL, 1);
			}
			for (int k = 10; k < 16; k++) {
				m_grid[i][j][k] = Voxel(glm::ivec3{i, j, k}, AIR, 1);
			}
		}
	}
}

void VoxelGrid::UpdateVoxel(glm::ivec3 index, voxelType type) {
	m_grid[index.x][index.y][index.z].UpdateType(type);
}

void VoxelGrid::Print() const {
	for (int i = 0; i < m_countX; i++) {
		for (int j = 0; j < m_countY; j++) {
			for (int k = 0; k < m_countZ; k++) {
				ivec3 index = m_grid[i][j][k].GetIndex();
				std::cout << "{" << index.x << ", " << index.y << ", " << index.z << ", " << m_grid[i][j][k].GetType() << "}, ";
			}
			cout << endl;
		}
		cout << "========================================================" << endl;
	}
}


std::string Voxel::GetType() const
{
	if (m_type == TERRAIN) return "Terrain";
	if (m_type == TALL_CELL) return "TallCell";
	if (m_type == REG_CELL) return "RegCell";
	if (m_type == AIR) return "Air";

}


HeightMap::HeightMap(glm::vec3 origin, glm::vec3 farEnd, float width) {
	m_width = width;
	m_gridCountX = (farEnd.x - origin.x) / width;
	m_gridCountY = (farEnd.y - origin.y) / width;

	m_grid = vector<vector<cellParams>> (m_gridCountX, vector<cellParams> (m_gridCountY));

	for(int i = 0; i < m_gridCountX; i++) {
		for (int j = 0; j < m_gridCountY; j++) {
			m_grid[i][j] = {0.f, 10, 0.f};
		}
	}

}