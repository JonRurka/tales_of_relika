#pragma once

#include "dynamic_compute.h"

#include "VoxelComputeProgram.h"

#include <vector>
#include <unordered_map>

using namespace VoxelEngine;


class HeightmapGenerator {
public:

	HeightmapGenerator(
		IComputeController* controller,
		int size_x, int size_z,
		IComputeBuffer* static_settings,
		int max_columns, 
		std::string extension, IComputeProgram::FileType type
	);

	bool Spawn_Column(glm::ivec2 column_coord);

	void Despawn_Column(glm::ivec2 column_coord);

	bool Has_Column(glm::ivec2 column_coord);

	IComputeBuffer* Height_Data() { return m_out_height_data; }
	IComputeBuffer* Biome_Data() { return m_out_biome_data; }

private:

	struct Run_Settings {
		glm::ivec4 Location;
	};

	int m_num_active{ 0 };

	int m_max_columns{ 0 };
	int m_size_x, m_size_z;
	int m_WorkGroups{ 16 };

	int m_current_index{ 0 };

	glm::ivec4* m_column_offsets{ nullptr };
	std::unordered_map<int, int> m_column_map;

	const std::string PROGRAM_GEN_HEIGHTMAP = "";
	VoxelComputeProgram* m_program_gen_heightmap{ nullptr };

	IComputeBuffer* m_in_static_settings_buffer{ nullptr };
	IComputeBuffer* m_in_run_settings_buffer{ nullptr };

	IComputeBuffer* m_in_column_offsets{ nullptr };

	IComputeBuffer* m_out_height_data{ nullptr };
	IComputeBuffer* m_out_biome_data{ nullptr };

	IComputeController* m_controller{nullptr};

	int find_next_index();

	void evaluate_heightmap(int col_x, int col_y, int hash, int index);

	inline static const std::string LOG_LOC{ "HEIGHTMAP_GENERATOR" };
};