#include "StructureDataStorage.h"

#include "Utilities.h"

#define GRID_PADDING 2
#define GRID_OFFSET 1

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
	m_chunk_data[chunk_hash(chunk_coord)] = new uint32_t[m_total_size];
	return true;
}

void StructureDataStorage::Despawn_Chunk(glm::ivec3 chunk_coord)
{
	if (!Has_Chunk(chunk_coord)) {
		return;
	}
	int hash = chunk_hash(chunk_coord);
	delete[] m_chunk_data[hash];
	m_chunk_data.erase(hash);
}

void StructureDataStorage::Set_Data(glm::ivec3 chunk_coord, uint32_t* data)
{
	if (Has_Chunk(chunk_coord)) {
		return;
	}

	memcpy(m_chunk_data[chunk_hash(chunk_coord)], data, m_total_size * sizeof(uint32_t));
}

void StructureDataStorage::Set_Data(glm::ivec3 chunk_coord, std::vector<uint32_t> data)
{
	memcpy(m_chunk_data[chunk_hash(chunk_coord)], data.data(), m_total_size * sizeof(uint32_t));
}

uint32_t* StructureDataStorage::Get_Data_ptr(glm::ivec3 chunk_coord)
{
	if (!Has_Chunk(chunk_coord)) {
		return nullptr;
	}

	return m_chunk_data[chunk_hash(chunk_coord)];
}

std::vector<uint32_t> StructureDataStorage::Get_Data(glm::ivec3 chunk_coord)
{
	if (!Has_Chunk(chunk_coord)) {
		return std::vector<uint32_t>();
	}

	uint32_t* data = m_chunk_data[chunk_hash(chunk_coord)];
	size_t size = m_total_size;
	return std::vector<uint32_t>(data, data + size);
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

int StructureDataStorage::chunk_hash(glm::ivec3 chunk_coord)
{
	return Utilities::Hash_Chunk_Coord(chunk_coord);
}
