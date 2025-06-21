#include "TerrainModifications.h"

#include "game_engine.h"

#define GRID_PADDING 2
#define GRID_OFFSET 1

namespace {
	uint32_t C_2D_to_1D(int x, int y, uint32_t width) {
		return (y * width + x);
	}

	uint32_t C_3D_to_1D(int x, int y, int z, uint32_t width, uint32_t height) {
		return z * width * height + y * width + x;
	}
}

TerrainModifications::TerrainModifications(IComputeController* controller, int size_x, int size_y, int size_z, int max_chunks) :
	m_controller{ controller },
	m_size_x{ size_x }, m_size_y{ size_y }, m_size_z{ size_z },
	m_max_chunks{ max_chunks }
{
	m_chunk_offsets = new glm::ivec4[m_max_chunks];
	memset(m_chunk_offsets, 0, m_max_chunks * sizeof(glm::ivec4));

	m_total_size = (size_x + GRID_PADDING) * (size_y + GRID_PADDING) * (size_z + GRID_PADDING);
	int num_elements = m_total_size * m_max_chunks;

	int mem_req = 0;

	m_modification_data = m_controller->NewReadWriteBuffer(num_elements, sizeof(float) * 4);
	mem_req += num_elements * 4;

	Logger::LogInfo(LOG_POS("NEW"), "Created for %i elements for %i chunks utilizing %i bytes.", num_elements, max_chunks, mem_req);
}

void TerrainModifications::Finalize(IComputeBuffer* static_settings)
{
}

bool TerrainModifications::Spawn_Chunk(glm::ivec3 chunk_coord)
{
	int chunk_coord_hash = Utilities::Hash_Chunk_Coord(chunk_coord);
	if (m_chunk_map.contains(chunk_coord_hash)) {
		return false;
	}

	int index = find_next_index();

	if (index == -1) {
		Logger::LogError(LOG_POS("Spawn_Chunk"), "Chunk Limit Reached!");
		return false;
	}

	m_chunk_map[chunk_coord_hash] = index * m_total_size;
	//Logger::LogDebug(LOG_POS("Spawn_Column"), "(%i, %i, %i) given index %i.",
	//	chunk_coord.x, chunk_coord.y, chunk_coord_hash, index);

	return true;
}

void TerrainModifications::Set_Chunk_Data(glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, float iso, int type)
{
	int chunk_coord_hash = Utilities::Hash_Chunk_Coord(chunk_coord);
	if (!m_chunk_map.contains(chunk_coord_hash)) {
		return;
	}

	// TODO: set on-edge chunks

	glm::vec4 data = glm::vec4(1.0, iso, type, 0);
	int chunk_start_index = m_chunk_map[chunk_coord_hash];
	int v_idx = C_3D_to_1D(voxel_coord.x, voxel_coord.y, voxel_coord.z, m_size_x, m_size_y);
	m_modification_data->SetData(&data, chunk_start_index + v_idx, m_total_size);
}

void TerrainModifications::Set_Chunk_Data(glm::ivec3 chunk_coord, std::vector<glm::vec4> data)
{
	int chunk_coord_hash = Utilities::Hash_Chunk_Coord(chunk_coord);
	if (!m_chunk_map.contains(chunk_coord_hash)) {
		return;
	}

	// TODO: set on-edge chunks

	int chunk_start_index = m_chunk_map[chunk_coord_hash];
	m_modification_data->SetData(data.data(), chunk_start_index, m_total_size);
}

void TerrainModifications::Despawn_Chunk(glm::ivec3 column_coord)
{
	int chunk_coord_hash = Utilities::Hash_Chunk_Coord(column_coord);

	if (!m_chunk_map.contains(chunk_coord_hash)) {
		return;
	}

	int index = m_chunk_map[chunk_coord_hash];
	m_chunk_offsets[index].x = 0;
	m_num_active--;
}

bool TerrainModifications::Has_Chunk(glm::ivec3 chunk_coord)
{
	int chunk_coord_hash = Utilities::Hash_Chunk_Coord(chunk_coord);
	return m_chunk_map.contains(chunk_coord_hash);
}

int TerrainModifications::Get_Chunk_Data_Offset(glm::ivec3 chunk_coord)
{
	int chunk_coord_hash = Utilities::Hash_Chunk_Coord(chunk_coord);
	return m_chunk_map[chunk_coord_hash];
}

int TerrainModifications::Grid_Padding()
{
	return GRID_PADDING;
}

int TerrainModifications::Grid_Offset()
{
	return GRID_OFFSET;
}

int TerrainModifications::find_next_index()
{
	if (m_num_active >= m_max_chunks) {
		return -1;
	}

	int num_traversed = 0;
	do {
		if (m_chunk_offsets[m_current_index].x == 0) {
			m_chunk_offsets[m_current_index].x = 1;
			m_num_active++;
			return m_current_index;
		}
		m_current_index++;
		if (m_current_index >= m_max_chunks) {
			m_current_index == 0;
		}
		num_traversed++;
	} while (num_traversed < m_max_chunks);

	return -1;
}
