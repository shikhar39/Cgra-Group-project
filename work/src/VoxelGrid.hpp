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
		voxelType m_type;
		size_t m_height;
	public:
		Voxel(glm::ivec3 index, voxelType type, size_t height) : m_index(index), m_type(type), m_height(height) {};
		void UpdateType(voxelType type) { m_type = type; };
		void UpdateHeight(size_t height) { m_height = height; };
		Voxel() : Voxel({ 0,0,0 }, AIR, 1) {};
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
	void UpdateVoxel(glm::ivec3 index, voxelType type);
	glm::ivec3 GetGridSize() const { return glm::vec3{m_countX, m_countY, m_countZ}; }
	size_t GetGridTotalCount() const { return m_countX * m_countY * m_countZ; }
	void Print() const;

};

/*
class TallCellGrid {
public:
	float m_width;
	size_t m_cellCountX, m_cellCountY, m_cellCountZ;
	size_t m_regCellCount;
	
	std::vector<std::vector<size_t>> m_terrainHeight;
	std::vector<std::vector<size_t>> m_tallCellHeight;
	
	std::vector<std::vector<std::vector<float>>> m_pressure;
	std::vector<std::vector<std::vector<glm::vec3>>> m_velocity;
	std::vector<std::vector<std::vector<float>>> m_levelSet;


};
*/

struct cellParams {
	float velocity;
	float height;
	float terrainHeight;
};

class HeightMap {
	float m_width;
	size_t m_gridCountX, m_gridCountY;
	std::vector<std::vector<cellParams>> m_grid;
	public:
	HeightMap(glm::vec3 origin, glm::vec3 farEnd, float width);
	HeightMap() : HeightMap(glm::vec3{0.f, 0.f, 0.f}, glm::vec3{ 1.f, 1.f, 1.f }, 0.1f) {};

	void SetTerrainHeight(glm::ivec2 index, float height) {m_grid[index.x][index.y].terrainHeight = height; };

	cellParams Get(size_t x, size_t y) const { return m_grid[x][y]; }	
	glm::ivec2 GetGridSize() const { return glm::ivec2{m_gridCountX, m_gridCountY}; }
	size_t GetGridTotalCount() const { return m_gridCountX * m_gridCountY; }
	float GetGridWidth() const { return m_width; }
};