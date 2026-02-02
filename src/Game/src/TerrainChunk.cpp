#include "TerrainChunk.h"

#include "Stitch_VBO.h"
#include "WorldGenController.h"
#include "Opaque_Chunk_Material.h"

#define DRAW_DEBUG_BOX false
#define DRAW_DEBUG_COLLISION_BOX true
#define DEBUG_DRAW_VERTICES false
#define DESPAWN_ENABLE true
#define COLLISION_ENABLED true
#define COLLISION_DISTANCE 2

void TerrainChunk::Init()
{
	

}

void TerrainChunk::Init(std::weak_ptr<WorldGenController> controller, Stitch_VBO::Shared vbo_stitch)
{
	assert(!controller.expired());
	assert(vbo_stitch.get() != nullptr);

	m_controller = controller;
	m_vbo_stitch = vbo_stitch;

	m_collision_distance = COLLISION_DISTANCE;

	int max_vert = (int)Utilities::Vertex_Limit_Mode::Chunk_Max;
	m_voxel_opaque_mesh = Mesh::Create(max_vert * Stitch_VBO::Byte_Stride());

	m_opaque_chunk_obj = Instantiate("Cached Voxel Chunk - Opaque");
	m_opaque_chunk_obj.lock()->Get_Transform().Set_Verbos(false);
	//obj->Get_MeshRenderer()->Transparent(true);
	m_opaque_chunk_obj.lock()->Get_Transform().Position(glm::vec3(0.0, 1000.0, 0.0));
	m_opaque_chunk_obj.lock()->Get_MeshRenderer().Set_Material(std::static_pointer_cast<Material>(controller.lock()->Get_Chunk_Material()));
	m_opaque_chunk_obj.lock()->Get_MeshRenderer().Set_Mesh(m_voxel_opaque_mesh);
	m_opaque_chunk_obj.lock()->Add_Component<MeshCollider>();

	//controller.lock()->ChunkMeterSize()
	m_opaque_chunk_obj.lock()->Get_MeshRenderer().Active(false);
	m_opaque_chunk_obj.lock()->Get_MeshRenderer().FrustumCull(true);
	

	m_col_vert_data = new glm::vec4[max_vert];

	//Logger::LogDebug(LOG_POS("Init"), "New chunk initialied. (%i, %i, %i)", 
	//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
}

void TerrainChunk::Assign(glm::ivec3 chunk_coord)
{
	assert(!m_opaque_chunk_obj.expired());

	m_should_despawn = false;
	m_has_collision = false;
	m_server_loaded = false;

	m_test_timer = 10;

	m_chunk_coords = chunk_coord;
	Object().Name("Voxel Chunk (" + std::to_string(chunk_coord.x) + ", " + std::to_string(chunk_coord.y) + ", " + std::to_string(chunk_coord.z) + ")");
	m_opaque_chunk_obj.lock()->Name("Voxel Chunk - Opaque (" + std::to_string(chunk_coord.x) + ", " + std::to_string(chunk_coord.y) + ", " + std::to_string(chunk_coord.z) + ")");

	m_chunk_world_pos = WorldGenController::ChunkCoordToWorldPos(chunk_coord);

	Object().Get_Transform().Position(m_chunk_world_pos);
	m_opaque_chunk_obj.lock()->Get_Transform().Position(m_chunk_world_pos);

	m_opaque_chunk_obj.lock()->Get_MeshRenderer().Active(true);

	//MeshRenderer

	if (DRAW_DEBUG_BOX)
		draw_debug_cube();

	m_assigned = true;

	//Logger::LogDebug(LOG_POS("Assign"), "New chunk assigned. (%i, %i, %i)",
	//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
}

void TerrainChunk::Unassign()
{
	if (!m_assigned)
		return;

	assert(!m_opaque_chunk_obj.expired());

	m_assigned = false;
	m_server_loaded = false;

	m_voxel_opaque_mesh->Clear();
	m_voxel_opaque_mesh->SetBounds(AABB());
	
	m_opaque_chunk_obj.lock()->Get_MeshRenderer().Active(false);

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


// Called from process thread in terrain engine.
void TerrainChunk::Process_Mesh_Update(glm::ivec4 counts, MeshUpdateMode mode)
{
	if (!m_assigned) {
		return;
	}

	if (counts.x == 0) {
		return;
	}

	int max_vert = (int)Utilities::Vertex_Limit_Mode::Chunk_Max;

	mesh_update m_update;
	m_update.mesh = Mesh::CreateDummy();//Mesh::Create(max_vert * Stitch_VBO::Byte_Stride());
	m_update.mode = mode;
	m_update.count = counts.x;

	double vbo_time_start = Utilities::Get_Time();
	m_vbo_stitch->Process(*m_update.mesh.get(), counts, false, false); //mode == MeshUpdateMode::Graphic || mode == MeshUpdateMode::Both

	m_update.raw_vbo = m_vbo_stitch->Output_VBO_Vector(counts.x);
	//auto raw_vert_data = new float[0];
	//int vbo_size = counts.x * BYTE_STRIDE;
	//m_vbo_stitch->Output_VBO_Buffer()->GetData(raw_vert_data, vbo_size);
	//std::vector<float> raw_data(raw_vert_data, raw_vert_data + (counts.x * FLOAT_STRIDE));


	double vbo_time_end = Utilities::Get_Time();
	double vbo_time = (vbo_time_end - vbo_time_start) * 1000.0;

	
	double t_start = Utilities::Get_Time();
	if (mode == MeshUpdateMode::Collision || mode == MeshUpdateMode::Both) {
		auto col_vert_data = new glm::vec4[max_vert];

		IComputeBuffer* vert_buffer = m_vbo_stitch->Input_Vertex_Buffer();
		vert_buffer->GetData(col_vert_data, counts.x * sizeof(float) * 4);
		m_update.vert = std::vector<glm::vec4>(col_vert_data, col_vert_data + counts.x);
		m_update.tris = m_vbo_stitch->Triangle_Data();

		delete[] col_vert_data;
	}
	double t_end = Utilities::Get_Time();
	double get_vert_time = (t_end - t_start) * 1000.0;

	{
		std::lock_guard<mutex> lock(m_update_lock);
		m_mesh_update_queue.push(m_update);
	}
}

void TerrainChunk::process_updates()
{
	std::queue<mesh_update> dst;
	Utilities::ThreadSafeQueueDuplicate(m_mesh_update_queue, dst, m_update_lock);

	while (!dst.empty())
	{
		process_mesh_update_internal(dst.front());
		dst.pop();
	}

}

void TerrainChunk::process_mesh_update_internal(const mesh_update& update)
{
	if (!m_assigned) {
		return;
	}

	//Logger::LogDebug(LOG_POS("Process_Mesh_Update"), "Process update.");

	/*if (mode == MeshUpdateMode::Graphic || mode == MeshUpdateMode::Both) {
		Logger::LogDebug(LOG_POS("Process_Mesh_Update"), "Render %s: %d", 
			Object().Name().c_str(), counts.x);
	}*/
	MeshUpdateMode mode = update.mode;
	int count = update.count;
	//Mesh::Shared mesh = update.mesh;

	if (mode == MeshUpdateMode::Graphic || mode == MeshUpdateMode::Both) {
		m_voxel_opaque_mesh->Set_Vertex_Attributes(Stitch_VBO::Get_Vertex_Attributes());
		m_voxel_opaque_mesh->Set_Raw_Vertex_Data(update.raw_vbo, false);
		m_voxel_opaque_mesh->Activate();
	}

	double coll_time_start = Utilities::Get_Time();
	if (mode == MeshUpdateMode::Collision || mode == MeshUpdateMode::Both)
	{
		//IComputeBuffer* vert_buffer = m_vbo_stitch->Input_Vertex_Buffer();
		//update_collision_mesh(vert_buffer, m_vbo_stitch->Triangle_Data().data(), count);
		update_collision_mesh(update.vert, update.tris, count);
	}
	double coll_time_end = Utilities::Get_Time();
	double coll_time = (coll_time_end - coll_time_start) * 1000.0;

	if (mode == MeshUpdateMode::Collision)
	{
		//Logger::LogDebug(LOG_POS("Process_Mesh_Update"), "Mesh Extract: %lf ms, Apply Collider: %lf ms",
		//	vbo_time, coll_time);
	}
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

bool TerrainChunk::Collision_Available()
{
	if (!m_assigned) {
		return false;
	}

	assert(!m_controller.expired());

	glm::ivec3 target = m_controller.lock()->Target_Chunk();
	float chunk_dist = glm::distance(glm::vec3(m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z), glm::vec3(target.x, target.y, target.z));
	if (chunk_dist <= m_collision_distance) {
		//Logger::LogDebug(LOG_POS("Collision_Enabled"), "(%i, %i, %i) - (%i, %i, %i): %f",
		//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z, target.x, target.y, target.z, chunk_dist);
		return COLLISION_ENABLED;
	}
	return false;
}

int TerrainChunk::Should_Update()
{
	if (!m_assigned) {
		return -1;
	}

	if (m_controller.expired())
	{
		return -2;
	}

	if (!m_controller.lock()->Finished_Initial_Generation()) {
		return -3;
	}

	if (m_should_despawn) {
		return -4;
	}

	return 1;
}

void TerrainChunk::DisableCollision()
{
	if (m_mesh_collider.expired())
	{
		return;
	}

	m_mesh_collider.lock()->Destroy();
	m_mesh_collider.reset();
	m_has_collision = false;

	//Logger::LogInfo(LOG_POS("update_collision_mesh"), "Collision removed for chunk (%i, %i, %i)",
	//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
}

void TerrainChunk::Refresh(MeshUpdateMode mode)
{
	if (!m_assigned) {
		return;
	}
	assert(!m_controller.expired());
	m_controller.lock()->Refresh_Chunk(m_chunk_coords, (WorldGenController::MeshUpdateMode)mode);
}

void TerrainChunk::OnServerChunkLoaded()
{
	m_server_loaded = true;
	//draw_debug_cube(glm::vec3(1, 0, 0), 1000);
	//Logger::LogDebug(LOG_POS("OnServerChunkLoaded"), "Received chunk load (%d, %d, %d)",
	//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
}

void TerrainChunk::OnServerChunkUnloaded()
{
	m_server_loaded = false;
	//Logger::LogDebug(LOG_POS("OnServerChunkUnloaded"), "Received chunk unload (%d, %d, %d)",
	//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
}

void TerrainChunk::Update(float dt)
{
	if (!m_assigned) {
		return;
	}

	assert(!m_controller.expired());

	process_updates();

	if (!m_controller.lock()->Finished_Initial_Generation()) {
		//return;
	}

	if (m_should_despawn) {
		return;
	}

	if (m_server_loaded)
	{
		//draw_debug_cube(glm::vec3(1, 0, 0));
	}

	/*m_test_timer -= dt;
	if (m_test_timer <= 0 && m_has_collision)
	{
		Logger::LogInfo(LOG_POS("Update"), "1. Debug Disabling collision for (%i, %i, %i)...",
			m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
		m_collision_distance = -1;
	}*/

	if (Collision_Available() && !Collision_Enabled())
	{
		//Logger::LogDebug(LOG_POS("Update"), "(%d, %d): Enabling collider...", m_chunk_coords.x, m_chunk_coords.z);
		Enable_Collision();
		
	}
	else if (!Collision_Available() && Collision_Enabled())
	{
		DisableCollision();
	}

	if (Collision_Available() && Collision_Enabled())
	{
		if (DRAW_DEBUG_COLLISION_BOX)
		{
			//draw_debug_cube(glm::vec3(1, 0, 0));
		}
	}

	if (m_last_col_success)
	{
		//draw_debug_cube(glm::vec3(0, 1, 0));
	}
	else
	{
		//draw_debug_cube(glm::vec3(1, 0, 0));
	}

	if (Collision_Available() && !Collision_Enabled())
	{
		Logger::LogError(LOG_POS("Update"), "Collision available but not enabled! This shouldn't happen...");
	}

	if (DESPAWN_ENABLE) {
		m_should_despawn = test_despawn();
	}

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

	//glm::vec3 target_pos = m_controller.lock()->Target_Position();
	//glm::ivec3 target_chunk = WorldGenController::WorldPosToChunkCoord(target_pos);
	glm::ivec3 target_chunk = m_controller.lock()->Target_Chunk();

	glm::fvec2 target_chunk_f = glm::fvec2(target_chunk.x, target_chunk.z);
	glm::fvec2 chunk_coord_f = glm::fvec2(m_chunk_coords.x, m_chunk_coords.z);

	float dist = glm::distance(target_chunk_f, chunk_coord_f);
	if (dist > m_controller.lock()->Chunk_Radius())
	{
		m_controller.lock()->Despawn_Chunk(m_chunk_coords);
		//Logger::LogDebug(LOG_POS("test_despawn"), "Despawn chunk (%d, %d, %d)",
		//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
		return true;
	}

	return false;
}

void TerrainChunk::draw_debug_cube(glm::vec3 color, float time)
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
	
	Graphics::DrawDebugRay(m_chunk_world_pos + glm::vec3(size/2 + 0.1f, size/2, size/2), glm::vec3(0, 5, 0), color, time);

	Graphics::DrawDebugLine(edge[0], edge[1], color, time);
	Graphics::DrawDebugLine(edge[1], edge[2], color, time);
	Graphics::DrawDebugLine(edge[2], edge[3], color, time);
	Graphics::DrawDebugLine(edge[3], edge[0], color, time);

	Graphics::DrawDebugLine(edge[4], edge[5], color, time);
	Graphics::DrawDebugLine(edge[5], edge[6], color, time);
	Graphics::DrawDebugLine(edge[6], edge[7], color, time);
	Graphics::DrawDebugLine(edge[7], edge[4], color, time);

	Graphics::DrawDebugLine(edge[0], edge[4], color, time);
	Graphics::DrawDebugLine(edge[1], edge[5], color, time);
	Graphics::DrawDebugLine(edge[2], edge[6], color, time);
	Graphics::DrawDebugLine(edge[3], edge[7], color, time);
}

//void TerrainChunk::update_collision_mesh(IComputeBuffer* vert_buffer, unsigned int* tris_data, int num_vertices)
void TerrainChunk::update_collision_mesh(const std::vector<glm::vec4>& vert_buffer, const std::vector<unsigned int>& tris_data, int num_vertices)
{
	//return;
	if (num_vertices <= 0) {
		return;
	}

	double t_start = Utilities::Get_Time();
	glm::vec3 extent_size = m_controller.lock()->ChunkMeterSize();
	extent_size = glm::vec3(extent_size.x / 2, extent_size.y / 2, extent_size.z / 2);
	m_voxel_opaque_mesh->SetBounds(AABB(extent_size, extent_size));
	double t_end = Utilities::Get_Time();
	double bounds_time = (t_end - t_start) * 1000.0;

	
	
	/*if (!Collision_Available()) {
		Logger::LogDebug(LOG_POS("update_collision_mesh"), "(%d, %d, %d): collision not available: ", 
			m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
		return;
	}*/
	glm::ivec3 target = m_controller.lock()->Target_Chunk();
	float chunk_dist = glm::distance(glm::vec3(m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z), glm::vec3(target.x, target.y, target.z));
	if (chunk_dist > m_collision_distance) {
		//Logger::LogDebug(LOG_POS("update_collision_mesh"), "(%d, %d, %d): collision not available: %.2lf",
		//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z, chunk_dist);
		m_last_col_success = false;
		return;
	}
	else {
		m_last_col_success = true;
	}
	//draw_debug_cube(glm::vec3(1, 0, 0), 10000);

	//Graphics::DrawDebugRay(m_chunk_world_pos + extent_size, glm::vec3(0, 10, 0), glm::vec3(0, 0, 1), 10000);

	t_start = Utilities::Get_Time();
	if (m_mesh_collider.expired()) {
		assert(!m_opaque_chunk_obj.expired());
		m_mesh_collider = m_opaque_chunk_obj.lock()->Add_Component<MeshCollider>();
	}
	t_end = Utilities::Get_Time();
	double add_coll_time = (t_end - t_start) * 1000.0;

	//Logger::LogDebug(LOG_POS("update_collision_mesh"), "Set Collider (%i): (%i, %i, %i)",
	//	(collision_enabled ? 1:0),m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);

	//draw_debug_cube();

	t_start = Utilities::Get_Time();
	//vert_buffer->GetData(m_col_vert_data, num_vertices * sizeof(float) * 4);
	//std::vector<glm::vec4> vert(m_col_vert_data, m_col_vert_data + num_vertices);
	t_end = Utilities::Get_Time();
	double get_vert_time = (t_end - t_start) * 1000.0;

	//std::vector<unsigned int> tris(tris_data, tris_data + num_vertices);
	
	
	if (DEBUG_DRAW_VERTICES) {
		for (int i = 0; i < num_vertices; i++) {
			//if (m_chunk_coords.x != 0 || m_chunk_coords.z != 0) {
			//	vert[i].y -= 2.f;
			//}

			if (i % 20 != 0)
				continue;

			Graphics::DrawDebugRay(m_chunk_world_pos + (glm::vec3)vert_buffer[i], glm::vec3(0, 0.5f, 0), glm::vec3(0, 0, 1), 10000);
		}
	}

	//Logger::LogDebug(LOG_POS("update_collision_mesh"), "(%d, %d, %d): %d verts",
	//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z, num_vertices);

	//btVector3 min, max;

	t_start = Utilities::Get_Time();
	m_collision_mesh = Mesh::Create();
	m_collision_mesh->GPU_Flush(false);
	//m_collision_mesh->Indices(tris);
	m_collision_mesh->Vertices(vert_buffer);
	m_collision_mesh->Activate(true);
	t_end = Utilities::Get_Time();
	double create_mesh_time = (t_end - t_start) * 1000.0;

	t_start = Utilities::Get_Time();
	m_mesh_collider.lock()->SetMesh(m_collision_mesh);
	t_end = Utilities::Get_Time();
	double set_mesh_time = (t_end - t_start) * 1000.0;

	m_mesh_collider.lock()->Mass(0.0f);

	t_start = Utilities::Get_Time();
	//m_mesh_collider.lock()->Activate();
	t_end = Utilities::Get_Time();
	double activate_time = (t_end - t_start) * 1000.0;
	


	//Logger::LogDebug(LOG_POS("update_collision_mesh"), "1: %0.2lf, 2: %0.2lf, 3: %0.2lf, 4: %0.2lf, 5: %0.2lf, 6: %0.2lf",
	//	bounds_time, add_coll_time, get_vert_time, create_mesh_time, set_mesh_time, activate_time);

	//m_mesh_collider->RigidBody()->forceActivationState(DISABLE_DEACTIVATION);
	//m_mesh_collider.lock()->RigidBody().getAabb(min, max);

	//Logger::LogDebug(LOG_POS("update_collision_mesh"), "Floor Min:(%f, %f, %f), max:(%f, %f, %f)",
	//	min.x(), min.y(), min.z(), max.x(), max.y(), max.z());

	//Logger::LogInfo(LOG_POS("Update"), "Collision for (%i, %i, %i) enabled successfully.",
	//	m_chunk_coords.x, m_chunk_coords.y, m_chunk_coords.z);
	m_has_collision = true;

	//draw_debug_cube(glm::vec3(1, 0, 0), 10000);

}

