#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>


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
		Voxel() : Voxel({ 0,0,0 }, AIR) {};
		std::string const GetType();
		glm::ivec3 const GetIndex() { return m_index; };
	};


private:
	float m_width;
	unsigned int m_countX, m_countY, m_countZ;
	std::vector<std::vector<std::vector<Voxel>>> m_grid;

public:
	VoxelGrid(glm::vec3 origin, glm::vec3 farEnd, float width);
	VoxelGrid() : VoxelGrid(glm::vec3{0.f, 0.f, 0.f}, glm::vec3{ 1.f, 1.f, 1.f }, 0.1f) {};
	void UpdateVoxel(glm::ivec3 index, voxelType type);
	glm::ivec3 const GetGridSize() { return glm::vec3{m_countX, m_countY, m_countZ}; };	
	void const Print();

};