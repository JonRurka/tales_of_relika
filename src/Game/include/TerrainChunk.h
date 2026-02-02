#pragma once

#include "game_engine.h"
#include "Stitch_VBO.h"

#include <memory>

class Opaque_Chunk_Material;
class Stitch_VBO;
class WorldGenController;

namespace DynamicCompute { 
	namespace Compute {
		class IComputeBuffer;
	}
}

class TerrainChunk : public Component
{
	friend class WorldGenController;
public:
	typedef std::shared_ptr<TerrainChunk> Shared;
	typedef std::weak_ptr<TerrainChunk> Weak;

	enum MeshUpdateMode
	{
		Graphic = 1,
		Collision = 2,
		Both = 3,
	};

	void Init(std::weak_ptr<WorldGenController> controller, Stitch_VBO::Shared vbo_stitch);

	void Assign(glm::ivec3 chunk_coord);

	void Unassign();

	void Process_Mesh_Update(glm::ivec4 counts, MeshUpdateMode mode);

	void Modify_Point_ISO(glm::ivec3 local_voxel, float iso);

	void Modify_Point_Type(glm::ivec3 local_voxel, int type);

	bool Assigned() const { return m_assigned; }

	bool Collision_Available();

	void Enable_Collision() 
	{ 
		// to keep this from triggering more than once, as 
		// it might take another frame or two to actually enable.
		m_has_collision = true;

		// trigger regeneration of the chunk.
		Refresh(MeshUpdateMode::Collision);
	}

	bool Collision_Enabled() const { return m_has_collision; }

	int Should_Update();

	void DisableCollision();

	void Refresh(MeshUpdateMode mode);

	void OnServerChunkLoaded();

	void OnServerChunkUnloaded();

protected:
	void Init() override;

	void Update(float dt) override;

	void OnDestroy() override;

private:

	struct mesh_update {
		Mesh::Shared mesh;
		MeshUpdateMode mode;
		int count;

		std::vector<float> raw_vbo;
		std::vector<glm::vec4> vert;
		std::vector<unsigned int> tris;
	};

	glm::ivec3 m_chunk_coords;
	glm::fvec3 m_chunk_world_pos;
	glm::ivec4 m_counts;
	std::weak_ptr<WorldGenController> m_controller;
	Stitch_VBO::Shared m_vbo_stitch;
	bool m_assigned{ false };
	bool m_has_collision{ false };
	bool m_should_despawn{ false };
	bool m_server_loaded{ false };
	int m_collision_distance{ 0 };
	glm::vec4* m_col_vert_data{ nullptr };


	bool m_last_col_success{ false };

	double m_test_timer = 0;

	std::queue<mesh_update> m_mesh_update_queue;
	std::mutex m_update_lock;

	WorldObject::Weak m_opaque_chunk_obj;
	Mesh::Shared m_voxel_opaque_mesh;

	MeshCollider::Weak m_mesh_collider;
	Mesh::Shared m_collision_mesh;

	void process_updates();

	void process_mesh_update_internal(const mesh_update& update);

	void VoxelChanged(glm::ivec3 local_voxel, bool ISO_changed, float iso, bool Type_changed, int type);

	bool test_despawn();

	void draw_debug_cube(glm::vec3 color = glm::vec3(0, 1, 0), float time = 0);

	//void update_collision_mesh(IComputeBuffer* vert_buffer, unsigned int* tris_data, int num_vertices);
	void update_collision_mesh(const std::vector<glm::vec4>& vert_buffer, const std::vector<unsigned int>& tris_data, int num_vertices);

	inline static const std::string LOG_LOC{ "TERRAIN_CHUNK" };

};
