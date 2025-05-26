#include "HeightmapGenerator.h"

#include "game_engine.h"

#define BASE_RESOURCE_DIR "compute::heightmapEngine::"

HeightmapGenerator::HeightmapGenerator(
	IComputeController* controller, 
	int size_x, int size_z, 
	IComputeBuffer* static_settings, 
	int max_columns, 
	std::string extension, IComputeProgram::FileType type) :

	m_controller{ controller },
	m_size_x{size_x}, m_size_z{ size_z },
	m_in_static_settings_buffer{ static_settings },
	m_max_columns{ max_columns }
{
	m_column_offsets = new glm::ivec4[m_max_columns];
	memset(m_column_offsets, 0, m_max_columns * sizeof(glm::ivec4));

	m_program_gen_heightmap = new VoxelComputeProgram(m_controller, BASE_RESOURCE_DIR + PROGRAM_GEN_HEIGHTMAP + extension, m_WorkGroups, type);

	m_in_run_settings_buffer = m_controller->NewReadWriteBuffer(1, sizeof(Run_Settings));

	m_in_column_offsets = m_controller->NewReadWriteBuffer(1, sizeof(int) * 4);

	int total_size = (size_x + 1) * (size_z + 1);
	m_out_height_data = m_controller->NewReadWriteBuffer(total_size * m_max_columns, sizeof(int) * 4);
	m_out_biome_data = m_controller->NewReadWriteBuffer(total_size * m_max_columns, sizeof(int) * 4);

	m_program_gen_heightmap->AddBuffer(0, m_in_static_settings_buffer);
	m_program_gen_heightmap->AddBuffer(1, m_in_run_settings_buffer);
	m_program_gen_heightmap->AddBuffer(2, m_in_column_offsets);
	m_program_gen_heightmap->AddBuffer(3, m_out_height_data);
	m_program_gen_heightmap->AddBuffer(4, m_out_biome_data);

	m_program_gen_heightmap->Finalize();

}

bool HeightmapGenerator::Spawn_Column(glm::ivec2 column_coord)
{
	int column_coord_hash = Utilities::Hash_Chunk_Coord(glm::ivec3(column_coord, 0));
	if (m_column_map.contains(column_coord_hash)) {
		return;
	}

	int index = find_next_index();

	if (index == -1) {
		Logger::LogError(LOG_POS("Spawn_Column"), "Column Limit Reached!");
		return false;
	}

	m_column_map[column_coord_hash] = index;

	evaluate_heightmap(column_coord.x, column_coord.y, column_coord_hash, index);

	return true;
}

void HeightmapGenerator::Despawn_Column(glm::ivec2 column_coord)
{
	int column_coord_hash = Utilities::Hash_Chunk_Coord(glm::ivec3(column_coord, 0));

	if (!m_column_map.contains(column_coord_hash)) {
		return;
	}

	int index = m_column_map[column_coord_hash];
	m_column_offsets[index].x = 0;
	m_num_active--;
}

bool HeightmapGenerator::Has_Column(glm::ivec2 column_coord)
{
	int column_coord_hash = Utilities::Hash_Chunk_Coord(glm::ivec3(column_coord, 0));
	return m_column_map.contains(column_coord_hash);
}

int HeightmapGenerator::find_next_index()
{
	if (m_num_active >= m_max_columns) {
		return -1;
	}

	int num_traversed = 0;
	do {
		if (m_column_offsets[m_current_index].x == 0) {
			m_column_offsets[m_current_index].x == 1;
			m_num_active++;
			return m_current_index;
		}
		m_current_index++;
		if (m_current_index >= m_max_columns) {
			m_current_index == 0;
		}
		num_traversed++;
	} while (num_traversed < m_max_columns);

	return -1;
}

void HeightmapGenerator::evaluate_heightmap(int col_x, int col_y, int hash, int index)
{
	glm::ivec4 run_setting = glm::ivec4(col_x, col_y, hash, index);


}
