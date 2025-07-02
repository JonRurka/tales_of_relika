#pragma once

#include "game_engine.h"

#include <mutex>
#include <queue>

class StructureController;
namespace VoxelEngine {
	class CubeVoxelBuilder;
}

using namespace VoxelEngine;

class StructureChunk : public Component
{
	friend class StructureController;
public:

	void Init(StructureController* controller);

	void Assign(glm::ivec3 chunk_coord);

	void Unassign();

	void Process_Mesh_Update(CubeVoxelBuilder* builder);

	glm::ivec3 Chunk_Coord() {return m_chunk_coords;}

	bool Collision_Enabled();

protected:
	void Init() override;

	void Update(float dt) override;

private:

	glm::ivec3 m_chunk_coords;
	glm::fvec3 m_chunk_world_pos;
	StructureController* m_controller{ nullptr };
	bool m_assigned{ false };
	bool m_has_collision{ false };
	bool m_should_despawn{ false };
	bool m_update_collision_mesh{ false };

	struct Chunk_Mesh_Data {
		std::vector<glm::vec4> vertices;
		std::vector<glm::vec4> normals;
		std::vector<unsigned int> indices;
		glm::ivec4 counts;
	};

	std::vector<glm::vec4> m_vertex;
	std::vector<glm::vec4> m_normal;
	std::vector<glm::vec2> m_uv;
	std::vector<unsigned int> m_trianges;
	glm::ivec4 m_counts;
	std::mutex m_data_lock;

	std::queue<Chunk_Mesh_Data> m_chunk_process_queue;

	WorldObject* m_opaque_chunk_obj{ nullptr };
	Mesh* m_voxel_opaque_mesh{ nullptr };
	MeshCollider* m_mesh_collider{ nullptr };
	Mesh* m_collision_mesh{ nullptr };

	void VoxelChanged(glm::ivec3 local_voxel, bool Type_changed, uint32_t type);

	void apply_mesh_update();

	void update_collision_mesh();

	void draw_debug_cube();

	static Mesh::VertexAttributeList get_vertex_attributes();

	inline static const std::string LOG_LOC{ "STRUCTURE_CHUNK" };
};