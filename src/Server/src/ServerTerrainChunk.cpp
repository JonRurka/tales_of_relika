#include "ServerTerrainChunk.h"

#include "WorldTerrain.h"
#include "World.h"
#include "ColliderGenerator.h"

void ServerTerrainChunk::Init(WorldTerrain* controller)
{
	m_controller = controller;
	m_world = m_controller->Get_World();
	m_world_physics = m_world->Physics();
}

void ServerTerrainChunk::Update(float dt)
{
	if (!m_assigned)
		return;

	/*m_test_timer -= dt;
	if (m_test_timer <= 0)
	{
		//Logger::LogDebug(LOG_POS("Unassign"), "Debug Despawning chunk: (%d, %d, %d)",
		//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
		//m_usages = 0;
		//m_keep_alive = false;
	}*/
	
	process_collider();
	test_removal();
}

void ServerTerrainChunk::Assign(glm::ivec3 chunk_coord) 
{
	m_assigned = true;
	m_chunk_coords = chunk_coord;

	m_test_timer = 10;

	m_chunk_world_pos = m_controller->ChunkCoordToWorldPos(chunk_coord);
}

void ServerTerrainChunk::Unassign() 
{
	

	m_assigned = false;
	m_usages = 0;
	m_keep_alive = false;

	clear_opaque_collision();
}

void ServerTerrainChunk::Iterate()
{
	m_usages++;
}

void ServerTerrainChunk::Deiterate()
{
	m_usages = std::max(m_usages - 1, 0);
}

void ServerTerrainChunk::Process_Mesh_Update(std::vector<glm::vec4> vert, std::vector<unsigned int> tris, glm::ivec4 counts)
{
	if (vert.size() <= 0)
		return;

	ColliderGenerator::Request* req = ColliderGenerator::Push_Request(m_chunk_coords, vert, tris);
	m_col_request_num++;
	m_collider_requests[m_col_request_num] = req;
	


	//set_opaque_collider(vert, tris, counts);
}

void ServerTerrainChunk::VoxelChanged(glm::ivec3 local_voxel, bool ISO_changed, float iso, bool Type_changed, int type) 
{

}

void ServerTerrainChunk::process_collider()
{
	struct spawn_ready 
	{
		ColliderGenerator::Request* req;
		uint64_t id;
	};


	std::queue<spawn_ready> ready_chunks;
	for (const auto& pair : m_collider_requests)
	{
		if (pair.second->Ready())
		{
			spawn_ready p{ pair.second, pair.first };
			ready_chunks.push(p);
		}
	}

	while (!ready_chunks.empty()) 
	{
		spawn_ready pair = ready_chunks.front();
		ready_chunks.pop();

		m_collider_requests.erase(pair.id);

		if (pair.id <= m_last_applied_col) {
			ColliderGenerator::Release_Request(pair.req);
			continue;
		}

		apply_collider(pair.req);

		m_last_applied_col = pair.id;
	}
}

void ServerTerrainChunk::apply_collider(ColliderGenerator::Request* req)
{
	if (m_current_col_req != nullptr)
	{
		clear_opaque_collision();
		ColliderGenerator::Release_Request(m_current_col_req);
		m_current_col_req = nullptr;
	}

	if (!req->Valid()) {
		return;
	}


#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	m_opaque_shape = req->Mesh_Shape();

	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(m_chunk_world_pos.x, m_chunk_world_pos.y, m_chunk_world_pos.z));

	m_localInertia = btVector3(0.0f, 0.0f, 0.0f);
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0, myMotionState, m_opaque_shape, m_localInertia);
	m_opaque_rigidbody = new btRigidBody(rbInfo);
	m_world_physics->Add_Rigidbody(m_opaque_rigidbody);
#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)

	m_opaque_shape = req->Mesh_Shape();

	glm::vec3 t_pos = m_chunk_world_pos;
	RVec3 r_pos = RVec3(t_pos.x, t_pos.y, t_pos.z);
	m_opaque_rigidbody = m_world_physics->GetBodyInterface().CreateBody(BodyCreationSettings(m_opaque_shape, r_pos, Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING));
	//m_world_physics->GetBodyInterface().AddBody(m_opaque_rigidbody->GetID(), EActivation::Activate);
	m_world_physics->Add_Rigidbody(m_opaque_rigidbody);

#endif
	//Logger::LogDebug(LOG_POS("apply_collider"), "Applied collider with %i vertices.", req->Num_Verts());

	m_current_col_req = req;
}

void ServerTerrainChunk::test_removal()
{
	if (m_keep_alive)
		return;

	if (m_usages > 0)
		return;

	m_controller->Despawn_Chunk(m_chunk_coords);
}

void ServerTerrainChunk::clear_opaque_collision()
{
	if (m_opaque_rigidbody != nullptr) {
		m_world_physics->Remove_Rigidbody(m_opaque_rigidbody);
		m_opaque_rigidbody = nullptr;
	}
}


