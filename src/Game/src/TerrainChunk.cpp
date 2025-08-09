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

void TerrainChunk::Init(std::weak_ptr<WorldGenController> controller, Stitch_VBO::Shared vbo_stitch)
{
	m_controller = controller;
	m_vbo_stitch = vbo_stitch;

	int max_vert = (int)Utilities::Vertex_Limit_Mode::Chunk_Max;
	m_voxel_opaque_mesh = Mesh::Create(max_vert * Stitch_VBO::Byte_Stride());

	m_opaque_chunk_obj = Instantiate("Cached Voxel Chunk - Opaque");
	m_opaque_chunk_obj.lock()->Get_Transform().Set_Verbos(false);
	//obj->Get_MeshRenderer()->Transparent(true);
	m_opaque_chunk_obj.lock()->Get_Transform().Position(glm::vec3(0.0, 1000.0, 0.0));
	m_opaque_chunk_obj.lock()->Get_MeshRenderer().Set_Material(std::static_pointer_cast<Material>(controller.lock()->Get_Chunk_Material()));
	m_opaque_chunk_obj.lock()->Get_MeshRenderer().Set_Mesh(m_voxel_opaque_mesh);
	m_opaque_chunk_obj.lock()->Add_Component<MeshCollider>();

	m_col_vert_data = new glm::vec4[max_vert];

	//Logger::LogDebug(LOG_POS("Init"), "New chunk initialied. (%i, %i, %i)", 
	//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
}

void TerrainChunk::Assign(glm::ivec3 chunk_coord)
{
	assert(!m_opaque_chunk_obj.expired());

	m_assigned = true;
	m_should_despawn = false;
	m_has_collision = false;

	m_chunk_coords = chunk_coord;
	Object().Name("Voxel Chunk (" + std::to_string(chunk_coord.x) + ", " + std::to_string(chunk_coord.y) + ", " + std::to_string(chunk_coord.z) + ")");
	m_opaque_chunk_obj.lock()->Name("Voxel Chunk - Opaque (" + std::to_string(chunk_coord.x) + ", " + std::to_string(chunk_coord.y) + ", " + std::to_string(chunk_coord.z) + ")");

	m_chunk_world_pos = WorldGenController::ChunkCoordToWorldPos(chunk_coord);

	Object().Get_Transform().Position(m_chunk_world_pos);
	m_opaque_chunk_obj.lock()->Get_Transform().Position(m_chunk_world_pos);

	if (DRAW_DEBUG_BOX)
		draw_debug_cube();

	//Logger::LogDebug(LOG_POS("Assign"), "New chunk assigned. (%i, %i, %i)",
	//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
}

void TerrainChunk::Unassign()
{
	if (!m_assigned)
		return;

	assert(!m_opaque_chunk_obj.expired());

	m_assigned = false;

	if (!m_mesh_collider.expired()) {
		assert(!m_opaque_chunk_obj.expired());
		m_mesh_collider.lock()->Clear();
	}
	m_has_collision = false;

	Object().Name("Cached Voxel Chunk");
	m_opaque_chunk_obj.lock()->Name("Cached Voxel Chunk - Opaque");

	Object().Get_Transform().Position(glm::vec3(0.0, 1000.0, 0.0));
	m_opaque_chunk_obj.lock()->Get_Transform().Position(glm::vec3(0.0, 1000.0, 0.0));
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

	m_vbo_stitch->Process(*m_voxel_opaque_mesh.get(), counts, false);

	IComputeBuffer* vert_buffer = m_vbo_stitch->Input_Vertex_Buffer();
	update_collision_mesh(vert_buffer, m_vbo_stitch->Triangle_Data().data(), counts.x);
}

void TerrainChunk::Modify_Point_ISO(glm::ivec3 local_voxel, float iso)
{
	if (!m_assigned) {
		return;
	}
	assert(!m_controller.expired());
	WorldGenController::TerrainMod mod(local_voxel, iso);
	m_controller.lock()->Submit_Terrain_Modification(m_chunk_coords, mod);
}

void TerrainChunk::Modify_Point_Type(glm::ivec3 local_voxel, int type)
{
	if (!m_assigned) {
		return;
	}
	assert(!m_controller.expired());
	WorldGenController::TerrainMod mod(local_voxel, type);
	m_controller.lock()->Submit_Terrain_Modification(m_chunk_coords, mod);
}

bool TerrainChunk::Collision_Enabled()
{
	if (!m_assigned) {
		return false;
	}
	assert(!m_controller.expired());
	glm::ivec3 target = m_controller.lock()->Target_Chunk();
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
	assert(!m_controller.expired());
	m_controller.lock()->Refresh_Chunk(m_chunk_coords);
}

void TerrainChunk::Update(float dt)
{
	if (!m_assigned) {
		return;
	}
	assert(!m_controller.expired());

	if (!m_controller.lock()->Finished_Initial_Generation()) {
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

void TerrainChunk::OnDestroy()
{
	if (m_col_vert_data != nullptr) {
		delete[] m_col_vert_data;
		m_col_vert_data = nullptr;
	}
}

void TerrainChunk::VoxelChanged(glm::ivec3 local_voxel, bool ISO_changed, float iso, bool Type_changed, int type)
{
	//Logger::LogDebug(LOG_POS("VoxelChanged"), "Changed voxel.");

}

bool TerrainChunk::test_despawn()
{
	assert(!m_controller.expired());

	glm::vec3 target_pos = m_controller.lock()->Target_Position();
	glm::ivec3 target_chunk = WorldGenController::WorldPosToChunkCoord(target_pos);

	glm::fvec2 target_chunk_f = glm::fvec2(target_chunk.x, target_chunk.z);
	glm::fvec2 chunk_coord_f = glm::fvec2(m_chunk_coords.x, m_chunk_coords.z);

	float dist = glm::distance(target_chunk_f, chunk_coord_f);
	if (dist > m_controller.lock()->Chunk_Radius())
	{
		m_controller.lock()->Despawn_Chunk(m_chunk_coords);
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

		if (!m_mesh_collider.expired()) {
			m_mesh_collider.lock()->Destroy();
			m_mesh_collider.reset();
			m_has_collision = false;
			Logger::LogInfo(LOG_POS("update_collision_mesh"), "Collision removed for chunk (%i, %i, %i)",
				m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
		}
		return;
	}

	if (m_mesh_collider.expired()) {
		assert(!m_opaque_chunk_obj.expired());
		m_mesh_collider = m_opaque_chunk_obj.lock()->Add_Component<MeshCollider>();
	}

	//Logger::LogDebug(LOG_POS("update_collision_mesh"), "Set Collider (%i): (%i, %i, %i)",
	//	(collision_enabled ? 1:0),m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);

	//draw_debug_cube();

	vert_buffer->GetData(m_col_vert_data, num_vertices * sizeof(float) * 4);

	std::vector<unsigned int> tris(tris_data, tris_data + num_vertices);
	std::vector<glm::vec4> vert(m_col_vert_data, m_col_vert_data + num_vertices);

	if (DEBUG_DRAW_VERTICES) {
		for (int i = 0; i < num_vertices; i++) {
			Graphics::DrawDebugRay(vert[i], glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), 10000);
		}
	}

	btVector3 min, max;

	m_collision_mesh = Mesh::Create();
	//m_collision_mesh->Indices(tris);
	m_collision_mesh->Vertices(vert);
	m_collision_mesh->Activate();
	m_mesh_collider.lock()->SetMesh(m_collision_mesh);
	m_mesh_collider.lock()->Mass(0.0f);
	m_mesh_collider.lock()->Activate();
	//m_mesh_collider->RigidBody()->forceActivationState(DISABLE_DEACTIVATION);
	m_mesh_collider.lock()->RigidBody().getAabb(min, max);

	//Logger::LogDebug(LOG_POS("update_collision_mesh"), "Floor Min:(%f, %f, %f), max:(%f, %f, %f)",
	//	min.x(), min.y(), min.z(), max.x(), max.y(), max.z());

	//Logger::LogInfo(LOG_POS("Update"), "Collision for (%i, %i, %i) enabled successfully.",
	//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
	m_has_collision = true;

}

