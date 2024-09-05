#pragma once

#include <glm/glm.hpp>
#include <vector>

class VoxelGrid {
public:
	enum voxelType {
		TERRAIN,
		TALL_CELL,
		REG_CELL,
		AIR
	};

private:
	class Voxel {
		glm::ivec3 m_index;
		voxelType m_type;
	public:
		Voxel(glm::ivec3 index, voxelType type) : m_index(index), m_type(type) {};
		void UpdateType(voxelType type);
		//Voxel() = delete;
	};
	static float m_width;
	std::vector<std::vector<std::vector<Voxel>>> m_grid;



public:
	VoxelGrid(glm::vec3 origin, glm::vec3 farEnd, float width);
	VoxelGrid() : VoxelGrid(glm::vec3{0.f, 0.f, 0.f}, glm::vec3{ 1.f, 1.f, 1.f }, 0.1f) {};
	void UpdateVoxel(glm::ivec3 index, voxelType type);
	

};