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
			for (int k = 0; k < m_countZ; k++) {
				m_grid[i][j][k] = Voxel(glm::ivec3{i, j, k}, AIR);
			}
		}
	}
}

void VoxelGrid::UpdateVoxel(glm::ivec3 index, int type) {
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

void Voxel::UpdateType(int type){
	m_type = type;
}

std::string Voxel::GetType() const
{
	if (m_type == TERRAIN) return "Terrain";
	if (m_type == TALL_CELL) return "TallCell";
	if (m_type == REG_CELL) return "RegCell";
	if (m_type == AIR) return "Air";

}
