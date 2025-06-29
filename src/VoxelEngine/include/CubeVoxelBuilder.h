#pragma once

#include "shared_structures.h"

class StructureDataStorage;

namespace VoxelEngine {

	class CubeVoxelBuilder {
	public:

		void Init(ChunkSettings* settings);

		glm::dvec4 Render(
			ChunkRenderOptions* options,
			glm::vec4* out_vertex, 
			glm::vec4* out_normal, 
			glm::vec2* out_uv, 
			unsigned int* out_trianges, 
			glm::ivec4 counts);

	private:

		struct Static_Settings {
			glm::vec4 VoxelsPerMeter;
			glm::ivec4 ChunkMeterSize;
			glm::ivec4 ChunkSize;
			glm::ivec4 FullChunkSize;
			glm::vec4 skipDist;
			glm::vec4 half_;
			glm::vec4 SideLength;
		};

		struct Out_Data {
			glm::vec4* out_vertex;
			glm::vec4* out_normal;
			glm::vec2* out_uv;
			unsigned int* out_trianges;
		};

		Static_Settings m_static_settings{};
		

		StructureDataStorage* m_structure_data{ nullptr };

		void CalculateVariables();

		void process_block(uint32_t* data, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, Out_Data& out_data);

		void process_tile(uint32_t* data, glm::ivec2 block_info, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, int tile_index, Out_Data& out_data);

		uint16_t get_block_type(uint32_t block_value);
		uint8_t get_block_orientation(uint32_t block_value);
	};


}