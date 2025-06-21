#include "HeightmapGenerator.h"

#include "game_engine.h"

#define BASE_RESOURCE_DIR "compute::heightmapEngine::"

#define HEIGHTMAP_PADDING 2
#define HEIGHTMAP_OFFSET 1

HeightmapGenerator::HeightmapGenerator(
	IComputeController* controller, 
	int size_x, int size_z, 
	int max_columns, 
	std::string extension, IComputeProgram::FileType type) :

	m_controller{ controller },
	m_size_x{size_x}, m_size_z{ size_z },
	m_max_columns{ max_columns }
{
	m_column_offsets = new glm::ivec4[m_max_columns];
	memset(m_column_offsets, 0, m_max_columns * sizeof(glm::ivec4));

	m_program_gen_heightmap = new VoxelComputeProgram(m_controller, BASE_RESOURCE_DIR + PROGRAM_GEN_HEIGHTMAP + extension, m_WorkGroups, type);

	m_in_run_settings_buffer = m_controller->NewReadWriteBuffer(1, sizeof(Run_Settings));

	m_in_column_offsets = m_controller->NewReadWriteBuffer(1, sizeof(int) * 4);

	m_total_size = (size_x + HEIGHTMAP_PADDING) * (size_z + HEIGHTMAP_PADDING);
	int num_elements = m_total_size * m_max_columns;

	int mem_req = 0;

	m_out_height_data = m_controller->NewReadWriteBuffer(num_elements, sizeof(float));
	mem_req += num_elements * 4;

	m_out_height_neighboor_data = m_controller->NewReadWriteBuffer(num_elements, sizeof(float) * 4);
	mem_req += num_elements * 4 * 4;

	m_out_biome_data = m_controller->NewReadWriteBuffer(num_elements, sizeof(int));
	mem_req += num_elements * 4;
	

	Logger::LogInfo(LOG_POS("NEW"), "Created for %i elements for %i colums utilizing %i bytes.", num_elements, max_columns, mem_req);

}

void HeightmapGenerator::Finalize(IComputeBuffer* static_settings)
{
	m_in_static_settings_buffer = static_settings;

	m_program_gen_heightmap->AddBuffer(0, m_in_static_settings_buffer);
	m_program_gen_heightmap->AddBuffer(1, m_in_run_settings_buffer);
	m_program_gen_heightmap->AddBuffer(2, m_out_height_data);
	m_program_gen_heightmap->AddBuffer(3, m_out_height_neighboor_data);
	m_program_gen_heightmap->AddBuffer(4, m_out_biome_data);


	m_program_gen_heightmap->Finalize();
}

bool HeightmapGenerator::Spawn_Column(glm::ivec2 column_coord)
{
	int column_coord_hash = Utilities::Hash_Chunk_Coord(glm::ivec3(column_coord, 0));
	if (m_column_map.contains(column_coord_hash)) {
		return false;
	}

	int index = find_next_index();

	if (index == -1) {
		Logger::LogError(LOG_POS("Spawn_Column"), "Column Limit Reached!");
		return false;
	}

	m_column_map[column_coord_hash] = index;
	//Logger::LogDebug(LOG_POS("Spawn_Column"), "(%i, %i, %i) given index %i.",
	//	column_coord.x, column_coord.y, column_coord_hash, index);
	
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

int HeightmapGenerator::Get_Column_Data_Offset(glm::ivec2 column_coord)
{
	int column_coord_hash = Utilities::Hash_Chunk_Coord(glm::ivec3(column_coord, 0));
	return m_column_map[column_coord_hash];
}

int HeightmapGenerator::find_next_index()
{
	if (m_num_active >= m_max_columns) {
		return -1;
	}

	int num_traversed = 0;
	do {
		if (m_column_offsets[m_current_index].x == 0) {
			m_column_offsets[m_current_index].x = 1;
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
	m_in_run_settings_buffer->SetData(&run_setting);

	m_program_gen_heightmap->Execute(m_total_size, 0, 0);

	/*
	int col_offset_start = index * m_total_size;
	float* data = new float[m_total_size];
	m_out_height_data->GetData(data, col_offset_start, m_total_size);
	int data_index = C_2D_to_1D(35 + 0, 32 + 0, (m_size_z + HEIGHTMAP_PADDING));
	printf("(%i) CPU height: %f\n", data_index, data[0]);
	delete[] data;*/
}
