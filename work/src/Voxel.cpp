#include "Voxel.hpp"


float Voxel::m_width = 0.1f;

Voxel::Voxel() {
	m_index = { 0, 0, 0 };
	m_type = AIR;
}

std::vector<Voxel>& Voxel::discretize(glm::vec3 origin, glm::vec3 farEnd, float width = 0.1f) {
	std::vector<Voxel> voxel_list;
	m_width = width;

	int x_count = (farEnd.x - origin.x) / m_width;
	int y_count = (farEnd.y - origin.y) / m_width;
	int z_count = (farEnd.z - origin.z) / m_width;



	return voxel_list;
}
