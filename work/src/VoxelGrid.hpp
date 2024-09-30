#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

enum voxelType {
	TERRAIN = 0,
	TALL_CELL = 1,
	REG_CELL = 2,
	AIR = 3
};
struct Voxel {
	public:
		glm::ivec3 m_index;
		int m_type;
	public:
		Voxel(glm::ivec3 index, int type) : m_index(index), m_type(type) {};
		void UpdateType(int type);
		Voxel() : Voxel({ 0,0,0 }, AIR) {};
		std::string GetType() const;
		glm::ivec3 GetIndex() const { return m_index; };
	};

class VoxelGrid {


public:
	float m_width;
	unsigned int m_countX, m_countY, m_countZ;
	std::vector<std::vector<std::vector<Voxel>>> m_grid;

public:
	VoxelGrid(glm::vec3 origin, glm::vec3 farEnd, float width);
	VoxelGrid() : VoxelGrid(glm::vec3{0.f, 0.f, 0.f}, glm::vec3{ 1.f, 1.f, 1.f }, 0.1f) {};
	void UpdateVoxel(glm::ivec3 index, int type);
	glm::ivec3 GetGridSize() const { return glm::vec3{m_countX, m_countY, m_countZ}; }
	size_t GetGridTotalCount() const { return m_countX * m_countY * m_countZ; }
	void Print() const;

};