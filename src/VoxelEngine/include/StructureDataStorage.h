#pragma once

#include <vector>
#include <unordered_map>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
//#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/vec4.hpp>

class StructureDataStorage {
public:

	StructureDataStorage(
		int size_x, int size_Y, int size_z, int max_chunks
	);

	bool Spawn_Chunk(glm::ivec3 chunk_coord);

	void Despawn_Chunk(glm::ivec3 chunk_coord);

	void Set_Data(glm::ivec3 chunk_coord, uint32_t* data);

	void Set_Data(glm::ivec3 chunk_coord, std::vector<uint32_t> data);

	void Set_Data(glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, uint32_t data);

	uint32_t* Get_Data_ptr(glm::ivec3 chunk_coord);

	std::vector<uint32_t> Get_Data(glm::ivec3 chunk_coord);

	bool Has_Chunk(glm::ivec3 chunk_coord);

	size_t Data_Size();

	int Grid_Padding();
	int Grid_Offset();

private:

	int m_num_active{ 0 };

	int m_max_chunks{ 0 };
	int m_size_x, m_size_y, m_size_z;

	int m_current_index{ 0 };

	int m_total_size{ 0 };

	std::unordered_map<int, uint32_t*> m_chunk_data;

	int chunk_hash(glm::ivec3 chunk_coord);

};