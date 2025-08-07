#include "TerrainChunk.h"

#include "Stitch_VBO.h"
#include "WorldGenController.h"
#include "Opaque_Chunk_Material.h"

#define DRAW_DEBUG_BOX false
#define DEBUG_DRAW_VERTICES false
#define COLLISION_DISTANCE 2

void TerrainChunk::Init()
{
	

}

void TerrainChunk::Init(WorldGenController* controller, Stitch_VBO* vbo_stitch)
{
	m_controller = controller;
	m_vbo_stitch = vbo_stitch;

	int max_vert = (int)Utilities::Vertex_Limit_Mode::Chunk_Max;
	m_voxel_opaque_mesh = new Mesh(max_vert * Stitch_VBO::Byte_Stride());

	m_opaque_chunk_obj = Instantiate("Cached Voxel Chunk - Opaque");
	m_opaque_chunk_obj->Get_Transform()->Set_Verbos(false);
	//obj->Get_MeshRenderer()->Transparent(true);
	m_opaque_chunk_obj->Get_Transform()->Position(glm::vec3(0.0, 1000.0, 0.0));
	m_opaque_chunk_obj->Get_MeshRenderer()->Set_Material(controller->Get_Chunk_Material());
	m_opaque_chunk_obj->Get_MeshRenderer()->Set_Mesh(m_voxel_opaque_mesh);
	m_opaque_chunk_obj->Add_Component<MeshCollider>();

	//Logger::LogDebug(LOG_POS("Init"), "New chunk initialied. (%i, %i, %i)", 
	//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
}

void TerrainChunk::Assign(glm::ivec3 chunk_coord)
{
	m_assigned = true;

	m_chunk_coords = chunk_coord;
	Object()->Name("Voxel Chunk (" + std::to_string(chunk_coord.x) + ", " + std::to_string(chunk_coord.y) + ", " + std::to_string(chunk_coord.z) + ")");
	m_opaque_chunk_obj->Name("Voxel Chunk - Opaque (" + std::to_string(chunk_coord.x) + ", " + std::to_string(chunk_coord.y) + ", " + std::to_string(chunk_coord.z) + ")");

	m_chunk_world_pos = WorldGenController::ChunkCoordToWorldPos(chunk_coord);

	Object()->Get_Transform()->Position(m_chunk_world_pos);
	m_opaque_chunk_obj->Get_Transform()->Position(m_chunk_world_pos);

	if (DRAW_DEBUG_BOX)
		draw_debug_cube();

	//Logger::LogDebug(LOG_POS("Assign"), "New chunk assigned. (%i, %i, %i)",
	//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
}

void TerrainChunk::Unassign()
{
	if (!m_assigned)
		return;

	m_assigned = false;

	Object()->Name("Cached Voxel Chunk");
	m_opaque_chunk_obj->Name("Cached Voxel Chunk - Opaque");

	Object()->Get_Transform()->Position(glm::vec3(0.0, 1000.0, 0.0));
	m_opaque_chunk_obj->Get_Transform()->Position(glm::vec3(0.0, 1000.0, 0.0));
}

void TerrainChunk::Process_Mesh_Update(glm::ivec4 counts)
{
	if (!m_assigned) {
		return;
	}

	if (counts.x == 0) {
		return;
	}

	//Logger::LogDebug(LOG_POS("Process_Mesh_Update"), "Process update.");

	m_vbo_stitch->Process(m_voxel_opaque_mesh, counts, false);

	IComputeBuffer* vert_buffer = m_vbo_stitch->Input_Vertex_Buffer();
	update_collision_mesh(vert_buffer, m_vbo_stitch->Triangle_Data(), counts.x);
}

void TerrainChunk::Modify_Point_ISO(glm::ivec3 local_voxel, float iso)
{
	if (!m_assigned) {
		return;
	}
	WorldGenController::TerrainMod mod(local_voxel, iso);
	m_controller->Submit_Terrain_Modification(m_chunk_coords, mod);
}

void TerrainChunk::Modify_Point_Type(glm::ivec3 local_voxel, int type)
{
	if (!m_assigned) {
		return;
	}
	WorldGenController::TerrainMod mod(local_voxel, type);
	m_controller->Submit_Terrain_Modification(m_chunk_coords, mod);
}

bool TerrainChunk::Collision_Enabled()
{
	if (!m_assigned) {
		return false;
	}

	glm::ivec3 target = m_controller->Target_Chunk();
	float chunk_dist = glm::distance(glm::vec3(m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z), glm::vec3(target.x, target.y, target.z));
	if (chunk_dist <= COLLISION_DISTANCE) {
		//Logger::LogDebug(LOG_POS("Collision_Enabled"), "(%i, %i, %i) - (%i, %i, %i): %f",
		//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z, target.x, target.y, target.z, chunk_dist);
		return true;
	}
	return false;
}

void TerrainChunk::Refresh()
{
	if (!m_assigned) {
		return;
	}
	m_controller->Refresh_Chunk(m_chunk_coords);
}

void TerrainChunk::Update(float dt)
{
	if (!m_assigned) {
		return;
	}

	if (!m_controller->Finished_Initial_Generation()) {
		return;
	}

	if (m_should_despawn) {
		return;
	}

	if (!m_has_collision) {
		if (Collision_Enabled()) {
			//Logger::LogInfo(LOG_POS("Update"), "Enabling collision for (%i, %i, %i)...",
			//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
			Refresh();
			m_has_collision = true;
			
		}
	}


	m_should_despawn = test_despawn();

}

void TerrainChunk::VoxelChanged(glm::ivec3 local_voxel, bool ISO_changed, float iso, bool Type_changed, int type)
{
	//Logger::LogDebug(LOG_POS("VoxelChanged"), "Changed voxel.");

}

bool TerrainChunk::test_despawn()
{
	glm::vec3 target_pos = m_controller->Target_Position();
	glm::ivec3 target_chunk = WorldGenController::WorldPosToChunkCoord(target_pos);

	glm::fvec2 target_chunk_f = glm::fvec2(target_chunk.x, target_chunk.z);
	glm::fvec2 chunk_coord_f = glm::fvec2(m_chunk_coords.x, m_chunk_coords.z);

	float dist = glm::distance(target_chunk_f, chunk_coord_f);
	if (dist > m_controller->Chunk_Radius()) 
	{
		m_controller->Despawn_Chunk(m_chunk_coords);
		return true;
	}

	return false;
}

void TerrainChunk::draw_debug_cube()
{
	glm::ivec4 directionOffsets[8] =
	{
		glm::ivec4(0, 0, 1, 0),
		glm::ivec4(1, 0, 1, 0),
		glm::ivec4(1, 0, 0, 0),
		glm::ivec4(0, 0, 0, 0),
		glm::ivec4(0, 1, 1, 0),
		glm::ivec4(1, 1, 1, 0),
		glm::ivec4(1, 1, 0, 0),
		glm::ivec4(0, 1, 0, 0),
	};

	glm::vec3 edge[8];
	
	int size = 32;

	//Graphics::DrawDebugLine()
	for (int i = 0; i < 8; i++) {
		edge[i] = m_chunk_world_pos + glm::fvec3(directionOffsets[i].x * size, directionOffsets[i].y * size, directionOffsets[i].z * size);
	}
	

	Graphics::DrawDebugLine(edge[0], edge[1], glm::vec3(0, 1, 0), 100000);
	Graphics::DrawDebugLine(edge[1], edge[2], glm::vec3(0, 1, 0), 100000);
	Graphics::DrawDebugLine(edge[2], edge[3], glm::vec3(0, 1, 0), 100000);
	Graphics::DrawDebugLine(edge[3], edge[0], glm::vec3(0, 1, 0), 100000);

	Graphics::DrawDebugLine(edge[4], edge[5], glm::vec3(0, 1, 0), 100000);
	Graphics::DrawDebugLine(edge[5], edge[6], glm::vec3(0, 1, 0), 100000);
	Graphics::DrawDebugLine(edge[6], edge[7], glm::vec3(0, 1, 0), 100000);
	Graphics::DrawDebugLine(edge[7], edge[4], glm::vec3(0, 1, 0), 100000);

	Graphics::DrawDebugLine(edge[0], edge[4], glm::vec3(0, 1, 0), 100000);
	Graphics::DrawDebugLine(edge[1], edge[5], glm::vec3(0, 1, 0), 100000);
	Graphics::DrawDebugLine(edge[2], edge[6], glm::vec3(0, 1, 0), 100000);
	Graphics::DrawDebugLine(edge[3], edge[7], glm::vec3(0, 1, 0), 100000);
}

void TerrainChunk::update_collision_mesh(IComputeBuffer* vert_buffer, unsigned int* tris_data, int num_vertices)
{
	//return;
	if (num_vertices <= 0) {
		return;
	}

	bool collision_enabled = Collision_Enabled();
	if (!collision_enabled) {

		if (m_mesh_collider != nullptr) {
			m_mesh_collider->Destroy();
			m_mesh_collider = nullptr;
			m_has_collision = false;
			Logger::LogInfo(LOG_POS("update_collision_mesh"), "Collision removed for chunk (%i, %i, %i)",
				m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
		}
		return;
	}

	if (m_mesh_collider == nullptr) {
		m_mesh_collider = m_opaque_chunk_obj->Add_Component<MeshCollider>();
	}

	//Logger::LogDebug(LOG_POS("update_collision_mesh"), "Set Collider (%i): (%i, %i, %i)",
	//	(collision_enabled ? 1:0),m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);

	//draw_debug_cube();

	glm::vec4* vert_data = new glm::vec4[num_vertices];
	vert_buffer->GetData(vert_data, num_vertices * sizeof(float) * 4);

	std::vector<unsigned int> tris(tris_data, tris_data + num_vertices);
	std::vector<glm::vec4> vert(vert_data, vert_data + num_vertices);

	if (DEBUG_DRAW_VERTICES) {
		for (int i = 0; i < num_vertices; i++) {
			Graphics::DrawDebugRay(vert[i], glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), 10000);
		}
	}

	btVector3 min, max;

	m_collision_mesh = new Mesh();
	//m_collision_mesh->Indices(tris);
	m_collision_mesh->Vertices(vert);
	m_collision_mesh->Activate();
	m_mesh_collider->SetMesh(m_collision_mesh);
	m_mesh_collider->Mass(0.0f);
	m_mesh_collider->Activate();
	//m_mesh_collider->RigidBody()->forceActivationState(DISABLE_DEACTIVATION);
	m_mesh_collider->RigidBody()->getAabb(min, max);

	//Logger::LogDebug(LOG_POS("update_collision_mesh"), "Floor Min:(%f, %f, %f), max:(%f, %f, %f)",
	//	min.x(), min.y(), min.z(), max.x(), max.y(), max.z());

	//Logger::LogInfo(LOG_POS("Update"), "Collision for (%i, %i, %i) enabled successfully.",
	//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
	m_has_collision = true;

}

