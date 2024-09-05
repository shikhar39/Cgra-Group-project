#include "VoxelGrid.hpp"

using namespace std;
float VoxelGrid::m_width = 0.1f;


VoxelGrid::VoxelGrid(glm::vec3 origin, glm::vec3 farEnd, float width) {
	
	m_width = width;
	int count_x = (farEnd.x - origin.x) / width;
	int count_y = (farEnd.y - origin.y) / width;
	int count_z = (farEnd.z - origin.z) / width;

	//m_grid = vector<vector<vector<Voxel>>>(count_z, vector<vector<Voxel> >(count_y, vector <Voxel>(count_x)));
	m_grid.resize(count_x);
	for (int i = 0; i < count_x; ++i) {
		m_grid[i].resize(count_y);
		for (int j = 0; j < count_y; ++j) {
			m_grid[i][j].resize(count_z);
		}
	}
	for (int i = 0; i < count_x; i++) {
		for (int j = 0; j < count_y; j++) {
			for (int k = 0; k < count_z; k++) {
				m_grid[i][j][k] = Voxel(glm::ivec3{i, j, k}, AIR);
			}
		}
	}
}

void VoxelGrid::UpdateVoxel(glm::ivec3 index, voxelType type) {
	m_grid[index.x][index.y][index.z].UpdateType(type);
}

void VoxelGrid::Voxel::UpdateType(voxelType type){
	m_type = type;
}
