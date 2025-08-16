#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
//#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <unordered_map>

#include "WorldPhysics.h"
#include "ColliderGenerator.h"

class WorldTerrain;
class World;

class ServerTerrainChunk {
public:

	void Init(WorldTerrain* controller);

	void Update(float dt);

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
	WorldTerrain* m_controller{ nullptr };
	World* m_world{ nullptr };
	WorldPhysics* m_world_physics{ nullptr };
	bool m_assigned{ false };
	bool m_should_despawn{ false };
	int m_usages{ 0 };
	bool m_keep_alive{ false };

	std::unordered_map<uint64_t, ColliderGenerator::Request*> m_collider_requests;
	uint64_t m_col_request_num{ 0 };
	uint64_t m_last_applied_col{ 0 };
	ColliderGenerator::Request* m_current_col_req{ nullptr };


#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)

	btVector3 m_localInertia{ btVector3(0.0f, 0.0f, 0.0f) };
	btCollisionShape* m_opaque_shape{ nullptr };
	btTriangleIndexVertexArray* m_opaque_TriangleIndexVertexArray{ nullptr };
	btTriangleMesh* m_opaque_triangle_mesh{ nullptr };
	btRigidBody* m_opaque_rigidbody{ nullptr };

#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)

	Ref<MeshShapeSettings> m_opaque_shape;
	Body* m_opaque_rigidbody{ nullptr };

#endif


	void process_collider();
	void apply_collider(ColliderGenerator::Request* req);

	void test_removal();

	void clear_opaque_collision();

	inline static const std::string LOG_LOC{ "SERVER_TERRAIN_CHUNK" };
};