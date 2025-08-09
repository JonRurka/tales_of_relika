#include "StructureDataStorage.h"

#include "Utilities.h"

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

StructureDataStorage::StructureDataStorage(int size_x, int size_y, int size_z, int max_chunks) :
	m_size_x{size_x}, m_size_y{size_y}, m_size_z{size_z}, m_max_chunks{max_chunks}
{
	m_total_size = (size_x + GRID_PADDING) * (size_y + GRID_PADDING) * (size_z + GRID_PADDING);
}

bool StructureDataStorage::Spawn_Chunk(glm::ivec3 chunk_coord)
{
	if (Has_Chunk(chunk_coord)) {
		return false;
	}
	m_chunk_data[chunk_hash(chunk_coord)] = std::vector<uint32_t>(m_total_size, 0);
	return true;
}

void StructureDataStorage::Despawn_Chunk(glm::ivec3 chunk_coord)
{
	if (!Has_Chunk(chunk_coord)) {
		return;
	}
	int hash = chunk_hash(chunk_coord);
	m_chunk_data.erase(hash);
}

void StructureDataStorage::Set_Data(glm::ivec3 chunk_coord, uint32_t* data)
{
	if (!Has_Chunk(chunk_coord)) {
		return;
	}
	int hash = chunk_hash(chunk_coord);
	m_chunk_data[hash] = std::vector<uint32_t>(data, data + m_total_size);
}

void StructureDataStorage::Set_Data(glm::ivec3 chunk_coord, std::vector<uint32_t> data)
{
	if (!Has_Chunk(chunk_coord)) {
		return;
	}
	if (data.size() <= 0) {
		return;
	}

	int hash = chunk_hash(chunk_coord);
	m_chunk_data[hash] = data;
}

void StructureDataStorage::Set_Data(glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, uint32_t data)
{
	if (!Has_Chunk(chunk_coord)) {
		return;
	}

	int f_size_x = m_size_x + GRID_PADDING;
	int f_size_y = m_size_y + GRID_PADDING;

	int hash = chunk_hash(chunk_coord);

	int v_idx = C_3D_to_1D(voxel_coord.x, voxel_coord.y, voxel_coord.z, f_size_x, f_size_y);
	
	m_chunk_data[hash][v_idx] = data;
}

const uint32_t* StructureDataStorage::Get_Data_ptr(glm::ivec3 chunk_coord)
{
	if (!Has_Chunk(chunk_coord)) {
		return nullptr;
	}

	return m_chunk_data[chunk_hash(chunk_coord)].data();
}

std::vector<uint32_t> StructureDataStorage::Get_Data(glm::ivec3 chunk_coord)
{
	if (!Has_Chunk(chunk_coord)) {
		return std::vector<uint32_t>();
	}
	return m_chunk_data[chunk_hash(chunk_coord)];
}

uint32_t StructureDataStorage::Get_Data(glm::ivec3 chunk_coord, glm::ivec3 voxel_coord)
{
	if (!Has_Chunk(chunk_coord)) {
		return 0;
	}

	int f_size_x = m_size_x + GRID_PADDING;
	int f_size_y = m_size_y + GRID_PADDING;

	int hash = chunk_hash(chunk_coord);

	int v_idx = C_3D_to_1D(voxel_coord.x, voxel_coord.y, voxel_coord.z, f_size_x, f_size_y);

	return m_chunk_data[hash][v_idx];
}

bool StructureDataStorage::Has_Chunk(glm::ivec3 chunk_coord)
{
	int chunk_coord_hash = Utilities::Hash_Chunk_Coord(chunk_coord);
	return m_chunk_data.contains(chunk_coord_hash);
}

size_t StructureDataStorage::Data_Size()
{
	return m_total_size;
}

int StructureDataStorage::Grid_Padding()
{
	return GRID_PADDING;
}

int StructureDataStorage::Grid_Offset()
{
	return GRID_OFFSET;
}

int StructureDataStorage::chunk_hash(glm::ivec3 chunk_coord)
{
	return Utilities::Hash_Chunk_Coord(chunk_coord);
}
