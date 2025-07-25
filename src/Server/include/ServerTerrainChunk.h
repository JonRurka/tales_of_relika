#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
//#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/vec4.hpp>

#include <vector>

class WorldTerrain;

class ServerTerrainChunk {
public:

	ServerTerrainChunk();

	void Init(WorldTerrain* controller);

	void Assign(glm::ivec3 chunk_coord);

	void Iterate();
	void Deiterate();
	int Usages() { return m_usages; }
	void KeepAlive(bool val) { m_keep_alive = val; }
	bool KeepAlive() { return m_keep_alive; }
		
	void Unassign();

	void Process_Mesh_Update(std::vector<glm::vec4> vert, std::vector<unsigned int> tris, glm::ivec4 counts);

	void VoxelChanged(glm::ivec3 local_voxel, bool ISO_changed, float iso, bool Type_changed, int type);

private:

	glm::ivec3 m_chunk_coords;
	glm::fvec3 m_chunk_world_pos;
	glm::ivec4 m_counts;
	WorldTerrain* m_controller;
	bool m_assigned{ false };
	bool m_should_despawn{ false };
	int m_usages{ 0 };
	bool m_keep_alive{ false };

};