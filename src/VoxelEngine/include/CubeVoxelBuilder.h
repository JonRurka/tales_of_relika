#pragma once

#include "shared_structures.h"

class StructureDataStorage;

namespace VoxelEngine {

	class CubeVoxelBuilder {
	public:

		typedef int(*RequestTileTexturePtr)(int block_id, int tile_id, uint8_t block_orientation);
		typedef bool(*RequestCanRenderPtr)(int block_id);

		void Init(ChunkSettings* settings);

		void SetRequestTileTextureCallback(RequestTileTexturePtr cb) {
			m_request_tile_tex_delegate = cb;
		}

		void SetCanRenderCallback(RequestCanRenderPtr cb) {
			m_can_render_delegate = cb;
		}

		glm::dvec4 Render(
			ChunkRenderOptions* options,
			std::vector<glm::vec4>& out_vertex,
			std::vector<glm::vec4>& out_normal,
			std::vector<glm::vec2>& out_uv,
			std::vector<unsigned int>& out_trianges,
			glm::ivec4& counts);

		StructureDataStorage* Get_Data_Storage() { return m_structure_data; }

		static uint32_t New_Block_Data(uint32_t type, uint8_t orientation);

		static uint32_t Set_Block_Orientation(uint32_t& init_value, uint8_t val);
		static uint32_t Set_Block_Type(uint32_t& init_value, uint32_t val);

		static uint32_t Get_Block_Type(uint32_t block_value);
		static uint8_t Get_Block_Orientation(uint32_t block_value);

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
			//glm::vec4* out_vertex;
			//glm::vec4* out_normal;
			//glm::vec2* out_uv;
			//unsigned int* out_trianges;
			glm::ivec4& out_counts;

			std::vector<glm::vec4>& out_vertex;
			std::vector<glm::vec4>& out_normal;
			std::vector<glm::vec2>& out_uv;
			std::vector<unsigned int>& out_trianges;

			Out_Data(
				glm::ivec4& counts,
				std::vector<glm::vec4>& vertex,
				std::vector<glm::vec4>& normal,
				std::vector<glm::vec2>& uv,
				std::vector<unsigned int>& trianges
			) :
				out_counts{ counts },
				out_vertex{ vertex },
				out_normal{ normal },
				out_uv {uv},
				out_trianges{ trianges }
			{ }
		};

		Static_Settings m_static_settings{};
		
		RequestTileTexturePtr m_request_tile_tex_delegate{nullptr};
		RequestCanRenderPtr m_can_render_delegate{ nullptr };

		StructureDataStorage* m_structure_data{ nullptr };

		void CalculateVariables();

		void process_block(uint32_t* data, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, Out_Data& out_data);

		void process_tile(uint32_t* data, glm::ivec2 block_info, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, int tile_index, Out_Data& out_data);

		void process_tile_x_plus(uint32_t* data, glm::ivec2 block_info, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, Out_Data& out_data);
		void process_tile_x_neg(uint32_t* data, glm::ivec2 block_info, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, Out_Data& out_data);

		void process_tile_y_plus(uint32_t* data, glm::ivec2 block_info, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, Out_Data& out_data);
		void process_tile_y_neg(uint32_t* data, glm::ivec2 block_info, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, Out_Data& out_data);

		void process_tile_z_plus(uint32_t* data, glm::ivec2 block_info, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, Out_Data& out_data);
		void process_tile_z_neg(uint32_t* data, glm::ivec2 block_info, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, Out_Data& out_data);

		bool neighboor_filed(uint32_t* data, glm::ivec3 this_voxel, glm::ivec3 neightboor_offset);

		
		int get_block_image_id(glm::ivec2 info, int tile_id);
		bool render_enabled(glm::ivec2 info);


		inline static const std::string LOG_LOC{ "CUBE_VOXEL_BUILDER" };
	};


}