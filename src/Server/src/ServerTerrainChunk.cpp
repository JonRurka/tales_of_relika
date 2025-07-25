#include "ServerTerrainChunk.h"

ServerTerrainChunk::ServerTerrainChunk()
{
}

void ServerTerrainChunk::Init(WorldTerrain* controller)
{
	m_controller = controller;
}

void ServerTerrainChunk::Assign(glm::ivec3 chunk_coord) 
{
	m_assigned = true;

	m_chunk_coords = chunk_coord;
}

void ServerTerrainChunk::Iterate()
{
	m_usages++;
}

void ServerTerrainChunk::Deiterate()
{
	m_usages--;
}

void ServerTerrainChunk::Process_Mesh_Update(std::vector<glm::vec4> vert, std::vector<unsigned int> tris, glm::ivec4 counts)
{

}

void ServerTerrainChunk::VoxelChanged(glm::ivec3 local_voxel, bool ISO_changed, float iso, bool Type_changed, int type) 
{

}


