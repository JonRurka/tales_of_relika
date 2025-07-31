#include "ServerTerrainChunk.h"

#include "WorldTerrain.h"
#include "World.h"
#include "ColliderGenerator.h"

ServerTerrainChunk::ServerTerrainChunk()
{
}

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

	
	process_collider();
	test_removal();
}

void ServerTerrainChunk::Assign(glm::ivec3 chunk_coord) 
{
	m_assigned = true;
	m_chunk_coords = chunk_coord;

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

	m_opaque_shape = req->Mesh_Shape();

	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(m_chunk_world_pos.x, m_chunk_world_pos.y, m_chunk_world_pos.z));

	m_localInertia = btVector3(0.0f, 0.0f, 0.0f);
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0, myMotionState, m_opaque_shape, m_localInertia);
	m_opaque_rigidbody = new btRigidBody(rbInfo);
	m_world_physics->Add_Rigidbody(m_opaque_rigidbody);

	Logger::LogDebug(LOG_POS("apply_collider"), "Applied collider with %i vertices.", req->Num_Verts());

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

void ServerTerrainChunk::set_opaque_collider(std::vector<glm::vec4> vert, std::vector<unsigned int> tris, glm::ivec4 counts)
{
	//double start_time = Utilities::Get_Time();
	//auto start = std::chrono::high_resolution_clock::now();

	clear_opaque_collision();

	if (vert.size() <= 0) {
		return;
	}

	

	if (tris.size() > 0) {
		std::vector<glm::vec3> vert3 = Utilities::vec4_to_vec3_arr(vert);

		btIndexedMesh indexedMesh;
		indexedMesh.m_numTriangles = tris.size() / 3;
		indexedMesh.m_triangleIndexBase = (unsigned char*)tris.data();
		indexedMesh.m_triangleIndexStride = 3 * sizeof(unsigned int);
		indexedMesh.m_numVertices = vert.size();
		indexedMesh.m_vertexBase = (unsigned char*)vert3.data();
		indexedMesh.m_vertexStride = sizeof(glm::vec3);

		m_opaque_TriangleIndexVertexArray = new btTriangleIndexVertexArray();
		m_opaque_TriangleIndexVertexArray->addIndexedMesh(indexedMesh);

		m_opaque_shape = new btBvhTriangleMeshShape(m_opaque_TriangleIndexVertexArray, true, true);
		Logger::LogDebug(LOG_POS("set_opaque_collider"), "Created (index) collider with %i verts", vert.size());
	}
	else {
		
		auto start = std::chrono::high_resolution_clock::now();

		m_opaque_triangle_mesh = new btTriangleMesh();
		//m_triangle_mesh->addIndexedMesh(indexedMesh);
		glm::vec3 col = glm::vec3(0.0, 0.0, 0.0);

		for (int t = 0; t < vert.size() / 3; t++) {

			glm::vec4 v1 = vert[(t * 3) + 0];
			glm::vec4 v2 = vert[(t * 3) + 1];
			glm::vec4 v3 = vert[(t * 3) + 2];

			m_opaque_triangle_mesh->addTriangle(
				btVector3(v1.x, v1.y, v1.z),
				btVector3(v2.x, v2.y, v2.z),
				btVector3(v3.x, v3.y, v3.z)
			);

			//Graphics::DrawDebugRay(v1, glm::vec3(0, 0.1, 0), col, 10000);
			//Graphics::DrawDebugRay(v2, glm::vec3(0, 0.1, 0), col, 10000);
			//Graphics::DrawDebugRay(v3, glm::vec3(0, 0.1, 0), col, 10000);

			//col.z += (float)t / (float)(vert.size() / 3);

		}
		m_opaque_shape = new btBvhTriangleMeshShape(m_opaque_triangle_mesh, true, true);

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration<double>(end - start).count();

		//Logger::LogDebug(LOG_POS("set_opaque_collider"), "Created collider with %i verts in %f ms",
		//	vert.size(), (float)((duration) * 1000.0f));
	}

	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(m_chunk_world_pos.x, m_chunk_world_pos.y, m_chunk_world_pos.z));

	m_localInertia = btVector3(0.0f, 0.0f, 0.0f);
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0, myMotionState, m_opaque_shape, m_localInertia);
	m_opaque_rigidbody = new btRigidBody(rbInfo);
	m_world_physics->Add_Rigidbody(m_opaque_rigidbody);

	//double end_time = Utilities::Get_Time();
	//auto end = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration<double>(end - start).count();

	//Logger::LogDebug(LOG_POS("set_opaque_collider"), "Created collider with %i verts in %f ms",
	//	vert.size(), (float)((duration) * 1000.0f));
}

void ServerTerrainChunk::clear_opaque_collision()
{
	if (m_opaque_rigidbody != nullptr) {
		m_world_physics->Remove_Rigidbody(m_opaque_rigidbody);
		m_opaque_rigidbody = nullptr;
	}
}


