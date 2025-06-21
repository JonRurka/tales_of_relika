#pragma once

#include "dynamic_compute.h"

#include <vector>
#include <unordered_map>

using namespace DynamicCompute::Compute;

class TerrainModifications {
public:
	   
	TerrainModifications(
		IComputeController* controller,
		int size_x, int size_Y, int size_z,
		int max_chunks
	);

	void Finalize(IComputeBuffer* static_settings);

	bool Spawn_Chunk(glm::ivec3 chunk_coord);

	void Set_Chunk_Data(glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, float iso, int type);

	void Set_Chunk_Data(glm::ivec3 chunk_coord, std::vector<glm::vec4> data);

	void Despawn_Chunk(glm::ivec3 chunk_coord);

	IComputeBuffer* Modification_Data() { return m_modification_data; }

	bool Has_Chunk(glm::ivec3 chunk_coord);

	int Get_Chunk_Data_Offset(glm::ivec3 chunk_coord);

	int Grid_Padding();
	int Grid_Offset();

private:

	int m_num_active{ 0 };

	int m_max_chunks{ 0 };
	int m_size_x, m_size_y, m_size_z;

	int m_current_index{ 0 };

	int m_total_size{ 0 };

	glm::ivec4* m_chunk_offsets{ nullptr };
	std::unordered_map<int, int> m_chunk_map;

	IComputeController* m_controller{ nullptr };
	IComputeBuffer* m_modification_data{nullptr};

	int find_next_index();

	inline static const std::string LOG_LOC{ "TERRAIN_MODIFICATIONS" };
};
