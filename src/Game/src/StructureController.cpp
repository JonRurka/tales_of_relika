#include "StructureController.h"

#include "CubeVoxelBuilder.h"
#include "StructureChunk.h"
#include "Opaque_Structure_Chunk_Material.h"
#include "Material_Types.h"
#include "Block_Type.h"
#include "StructureDataStorage.h"

#include <thread>

namespace {
	int floor_to_int(float val) {
		return static_cast<int>(std::floor(val));
	}

	uint32_t C_3D_to_1D(int x, int y, int z, uint32_t width, uint32_t height) {
		return z * width * height + y * width + x;
	}
}

StructureController* StructureController::m_Instance{nullptr};

StructureController::StructureMod::StructureMod(glm::ivec3 voxel, uint32_t type)
{
	Voxel = voxel;
	Type = type;
	Change_Type = true;
}

int StructureController::Hash_Chunk(glm::ivec3 chunk)
{
	return Utilities::Hash_Chunk_Coord(chunk);
}

void StructureController::Init()
{
	m_Instance = this;

	Logger::LogInfo(LOG_POS("Init"), "Initialized");

	m_half = 0;// ((1 / m_voxelsPerMeter) / 2.0);

	m_chunk_opaque_mat = new Opaque_Structure_Chunk_Material();
	//m_chunk_opaque_mat->setTexture("diffuse", m_diffuse_texture_array);
	m_chunk_opaque_mat->setTexture("diffuse", Material_Types::Instance()->Terrain_Diffuse_Texture_Array());
	m_chunk_opaque_mat->setTexture("normal_maps", Material_Types::Instance()->Terrain_Normal_Texture_Array());
	m_chunk_opaque_mat->SetVec3("material.ambientColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_chunk_opaque_mat->SetVec3("material.diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_chunk_opaque_mat->SetVec2("material.scale", glm::vec2(1.0f, 1.0f));
	m_chunk_opaque_mat->setFloat("material.shininess", 8.0f);
	m_chunk_opaque_mat->setFloat("material.specular_intensity", 1.0f);
	m_chunk_opaque_mat->SetVec3("globalAmbientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_chunk_opaque_mat->setFloat("globalAmbientIntensity", 0.01f);

	initialize_voxel_engine();

	start();
}

void StructureController::Update(float dt)
{



}

void StructureController::start()
{
	create_chunk_cache();
	m_process_thread = std::thread(Run, this);
}

void StructureController::Run(StructureController* inst)
{
	inst->run_loop();
}

void StructureController::run_loop()
{
	m_async_run = true;
	while (m_async_run) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		async_update();
	}
}

void StructureController::async_update()
{
	process_additions();
	process_deletions();
	process_modifications();
}

void StructureController::initialize_voxel_engine()
{
	settings.GetSettings()->setFloat("voxelsPerMeter", m_voxelsPerMeter);
	settings.GetSettings()->setInt("chunkMeterSizeX", m_chunkMeterSizeX);
	settings.GetSettings()->setInt("chunkMeterSizeY", m_chunkMeterSizeY);
	settings.GetSettings()->setInt("chunkMeterSizeZ", m_chunkMeterSizeZ);

	m_chunk_size_x = m_chunkMeterSizeX * m_voxelsPerMeter;
	m_chunk_size_y = m_chunkMeterSizeY * m_voxelsPerMeter;
	m_chunk_size_z = m_chunkMeterSizeZ * m_voxelsPerMeter;

	m_builder = new CubeVoxelBuilder();
	m_builder->Init(&settings);
	m_builder->SetRequestTileTextureCallback(Block_Type::GetTileTexture_Callback);
	m_builder->SetCanRenderCallback(Block_Type::CanRender_Callback);

	m_data_storage = m_builder->Get_Data_Storage();

}

void StructureController::process_additions()
{
	bool has_items = !m_create_queue.empty();

	if (!has_items)
		return;

	double start = Utilities::Get_Time();
	double end = start;
	double timer = (end - start);

	while (has_items && timer < m_process_time_ms) {
		//has_items = process_batch();
		ChunkCreationRequest req = m_create_queue.front();
		m_create_queue.pop();

		ChunkRef chunk = req.Chunk;

		m_data_storage->Spawn_Chunk(chunk.chunk_coord);
		m_data_storage->Set_Data(chunk.chunk_coord, req.Initial_Data);

		process_chunk(chunk);

		Logger::LogDebug(LOG_POS("process_additions"), "created chunk.");

		has_items = !m_create_queue.empty();
		end = Utilities::Get_Time();
		timer = (end - start) * 1000.0;
	}

}

void StructureController::process_deletions()
{
	while (!m_delete_queue.empty()) {
		glm::ivec3 chunk_coord = m_delete_queue.front();
		m_delete_queue.pop();

		if (!chunk_exists(chunk_coord)) {
			continue;
		}

		ChunkRef chunk = get_chunk(chunk_coord);
		chunk.chunk_comp->Unassign();
		remove_chunk(chunk_coord);
		m_cached_chunks.push(chunk);
	}
}

void StructureController::process_modifications()
{
	while (!m_terrain_change_queue.empty())
	{
		//Logger::LogDebug(LOG_POS("process_modifications"), "Items in mod queue: %i", m_terrain_change_queue.size());

		StructureModEntry entry = m_terrain_change_queue.front();
		m_terrain_change_queue.pop();

		glm::ivec3 chunk = entry.chunk;
		std::vector<StructureMod> changes = entry.changes;

		if (!Chunk_Exists(chunk)) {
			continue;
		}

		for (const auto& v_change : changes)
		{
			if (v_change.Change_Type) {
				glm::ivec3 voxel_coord = v_change.Voxel;
				voxel_coord.x += m_data_storage->Grid_Offset();
				voxel_coord.y += m_data_storage->Grid_Offset();
				voxel_coord.z += m_data_storage->Grid_Offset();
				Logger::LogDebug(LOG_POS("process_modifications"), "Shifted voxel: (%i, %i, %i)",
					voxel_coord.x, voxel_coord.y, voxel_coord.z);
				m_data_storage->Set_Data(chunk, voxel_coord, v_change.Type);
			}
			else {
				Logger::LogDebug(LOG_POS("process_modifications"), "Change not accepted.");
			}
		}

		get_chunk(chunk).chunk_comp->Process_Mesh_Update(m_builder);
	}
}

void StructureController::process_chunk(ChunkRef chunk)
{
	double start_time = Utilities::Get_Time();

	//glm::dvec4 render_times = m_builder->Render(&render_options,);
	chunk.chunk_comp->Process_Mesh_Update(m_builder);

	double end_time = Utilities::Get_Time();
	double duration_ms = (end_time - start_time) * 1000.0;

}

bool StructureController::queue_chunk_create(glm::ivec3 chunk_coord)
{
	return queue_chunk_create(chunk_coord, std::vector<uint32_t>());
}

bool StructureController::queue_chunk_create(glm::ivec3 chunk_coord, std::vector<StructureMod> values) 
{
	std::vector<uint32_t> data(m_data_storage->Data_Size(), 0);
	//data.reserve(m_data_storage->Data_Size());
	//std::fill(data.begin(), data.end(), 0);

	for (const auto& val : values) {
		Logger::LogDebug(LOG_POS("queue_chunk_create"), "Initial Voxel: (%i, %i, %i) (%i, %i, %i)",
			chunk_coord.x, chunk_coord.y, chunk_coord.z, val.Voxel.x, val.Voxel.y, val.Voxel.z);
		int v_idx = C_3D_to_1D(
			val.Voxel.x + m_data_storage->Grid_Offset(),
			val.Voxel.y + m_data_storage->Grid_Offset(),
			val.Voxel.z + m_data_storage->Grid_Offset(),
			m_chunk_size_x + m_data_storage->Grid_Padding(),
			m_chunk_size_y + m_data_storage->Grid_Padding()
		);
		data[v_idx] = val.Type;
	}

	return queue_chunk_create(chunk_coord, data);
}

bool StructureController::queue_chunk_create(glm::ivec3 chunk_coord, std::vector<uint32_t> data)
{
	int hash = Utilities::Hash_Chunk_Coord(chunk_coord.x, chunk_coord.y, chunk_coord.z);

	if (m_chunk_map.contains(hash)) {
		return false;
	}

	ChunkRef chunk = m_cached_chunks.front();
	m_cached_chunks.pop();

	chunk.chunk_comp->Assign(chunk_coord);
	chunk.chunk_coord = chunk_coord;

	m_chunk_map[hash] = chunk;

	ChunkCreationRequest req{};
	req.Chunk = chunk;
	req.Initial_Data = data;

	m_create_queue.push(req);
	Logger::LogDebug(LOG_POS("queue_chunk_create"), "Chunk creation queued.");

	return true;
}

void StructureController::queue_chunk_delete(glm::ivec3 chunk_coord)
{
	m_delete_queue.push(chunk_coord);
}

bool StructureController::chunk_exists(glm::ivec3 chunk_coord)
{
	int hash = Utilities::Hash_Chunk_Coord(chunk_coord.x, chunk_coord.y, chunk_coord.z);
	return m_chunk_map.contains(hash);
}

void StructureController::remove_chunk(glm::ivec3 chunk_coord)
{
	int hash = Utilities::Hash_Chunk_Coord(chunk_coord.x, chunk_coord.y, chunk_coord.z);
	m_chunk_map.erase(hash);
}

void StructureController::create_chunk_cache()
{
	double start = Utilities::Get_Time();

	for (int i = 0; i < m_max_cached_chunks; ++i) {
		ChunkRef chk = create_chunk_object();
		m_cached_chunks.push(chk);
	}

	double end = Utilities::Get_Time();
	Logger::LogDebug(LOG_POS("create_chunk_cache"), "Created %i cached chunks in %f ms.", m_max_cached_chunks, (end - start) * 1000);
}

StructureController::ChunkRef StructureController::create_chunk_object()
{
	WorldObject* obj = Instantiate("Cached Structure Chunk");
	obj->Get_Transform()->Set_Verbos(false);
	obj->Get_Transform()->Position(glm::vec3(0.0, 1000.0, 0.0));
	StructureChunk* comp = obj->Add_Component<StructureChunk>();
	comp->Init(this);

	ChunkRef chk{};
	chk.chunk_obj = obj;
	chk.chunk_comp = comp;
	//m_chunk_map[hash] = chk;

	return chk;
}

StructureController::ChunkRef StructureController::get_chunk(glm::ivec3 chunk_coord)
{
	int hash = Utilities::Hash_Chunk_Coord(chunk_coord.x, chunk_coord.y, chunk_coord.z);
	return m_chunk_map[hash];
}

void StructureController::Refresh_Chunk(glm::ivec3 chunk)
{
	StructureMod val(glm::vec3(0, 0, 0));
	Modify_Voxel(chunk, val, false);
}

void StructureController::Modify_Voxel_Type(glm::ivec3 voxel, uint32_t type)
{
	glm::ivec3 chunk = voxelToChunk(voxel);
	glm::ivec3 voxel_local = globalToLocalChunkCoord(chunk, voxel);

	if (!Chunk_Exists(chunk)) {
		Logger::LogDebug(LOG_POS("Modify_Voxel_Type"), "chunk no exist: (%i, %i, %i)",
			chunk.x, chunk.y, chunk.z);
		return;
	}

	Logger::LogDebug(LOG_POS("Modify_Voxel_Type"), "modify voxel: (%i, %i, %i) (%i, %i, %i)",
		chunk.x, chunk.y, chunk.z, voxel_local.x, voxel_local.y, voxel_local.z);

	get_chunk(chunk).chunk_comp->VoxelChanged(voxel_local, true, type);

	StructureMod mod(voxel_local, type);
	Modify_Voxel(chunk, mod);
}

void StructureController::Modify_Voxel(std::vector<StructureMod> values)
{
	std::unordered_map<int, std::vector<StructureMod>> mods;

	for (const auto& elem : values)
	{
		glm::ivec3 chunk = voxelToChunk(elem.Voxel);
		glm::ivec3 voxel_local = globalToLocalChunkCoord(chunk, elem.Voxel);

		//int chunk_hash = 
		int chunk_hash = Utilities::Hash_Chunk_Coord(chunk.x, chunk.y, chunk.z);
		if (!mods.contains(chunk_hash))
		{
			mods[chunk_hash] = std::vector<StructureMod>();
		}

		// TODO: call terrain chunk event

		StructureMod mod(voxel_local);
		mod.Change_Type = elem.Change_Type;
		mod.Type = elem.Type;
		mod.chunk_coord = chunk;
		mods[chunk_hash].push_back(mod);
	}

	for (const auto& pair : mods)
	{
		glm::ivec3 chunk = pair.second[0].chunk_coord;
		Modify_Voxel(chunk, pair.second);
	}
}

void StructureController::Modify_Voxel(glm::ivec3 chunk, StructureMod value, bool update_neighbor)
{
	std::vector<StructureMod> values;
	values.push_back(value);
	Modify_Voxel(chunk, values, update_neighbor);
}

void StructureController::Modify_Voxel(glm::ivec3 chunk, std::vector<StructureMod> values, bool update_neighbor)
{
	Submit_Structure_Modification(chunk, values);


	if (update_neighbor) {
		std::unordered_map<int, std::vector<StructureMod>> neighboor_updates;
		for (const auto& val : values)
		{
			glm::ivec3 voxel = val.Voxel;

			glm::ivec3 other_chunk;
			glm::ivec3 other_chunk_voxel;
			bool has_neighboor = false;

			if (voxel.x == 0) {
				other_chunk = glm::ivec3(chunk.x - 1, chunk.y, chunk.z);
				other_chunk_voxel = glm::ivec3(m_chunk_size_x, voxel.y, voxel.z);
				has_neighboor = true;

			}
			if (voxel.x == (m_chunk_size_x - 1)) {
				other_chunk = glm::ivec3(chunk.x + 1, chunk.y, chunk.z);
				other_chunk_voxel = glm::ivec3(-1, voxel.y, voxel.z);
				has_neighboor = true;
			}

			if (voxel.y == 0) {
				other_chunk = glm::ivec3(chunk.x, chunk.y - 1, chunk.z);
				other_chunk_voxel = glm::ivec3(voxel.x, m_chunk_size_y, voxel.z);
				has_neighboor = true;
			}
			if (voxel.y == (m_chunk_size_y - 1)) {
				other_chunk = glm::ivec3(chunk.x, chunk.y + 1, chunk.z);
				other_chunk_voxel = glm::ivec3(voxel.x, -1, voxel.z);
				has_neighboor = true;
			}

			if (voxel.z == 0) {
				other_chunk = glm::ivec3(chunk.x, chunk.y, chunk.z - 1);
				other_chunk_voxel = glm::ivec3(voxel.x, voxel.y, m_chunk_size_z);
				has_neighboor = true;
			}
			if (voxel.z == (m_chunk_size_z - 1)) {
				other_chunk = glm::ivec3(chunk.x, chunk.y, chunk.z + 1);
				other_chunk_voxel = glm::ivec3(voxel.x, voxel.y, -1);
				has_neighboor = true;
			}

			if (!has_neighboor) {
				continue;
			}

			StructureMod other_mod(other_chunk_voxel);
			other_mod.Type = val.Type;
			other_mod.Change_Type = val.Change_Type;
			other_mod.chunk_coord = other_chunk_voxel;

			int other_hash = Utilities::Hash_Chunk_Coord(other_chunk.x, other_chunk.y, other_chunk.z);
			if (!neighboor_updates.contains(other_hash))
			{
				neighboor_updates[other_hash] = std::vector<StructureMod>();
			}

			neighboor_updates[other_hash].push_back(other_mod);
		}

		for (const auto& pair : neighboor_updates) {
			glm::ivec3 other_chunk = pair.second[0].chunk_coord;
			Submit_Structure_Modification(other_chunk, pair.second);
		}

	}
}

void StructureController::Submit_Structure_Modification(glm::ivec3 chunk, StructureMod value)
{
	StructureModEntry entry{};
	entry.chunk = chunk;
	entry.changes.push_back(value);
	m_terrain_change_queue.push(entry);
}

void StructureController::Submit_Structure_Modification(glm::ivec3 chunk, std::vector<StructureMod> values)
{
	Logger::LogDebug(LOG_POS("Submit_Structure_Modification"), "Change: (%i, %i, %i)", 
		values[0].Voxel.x, values[0].Voxel.y, values[0].Voxel.z);
	StructureModEntry entry{};
	entry.chunk = chunk;
	entry.changes = values;
	m_terrain_change_queue.push(entry);
}


glm::ivec3 StructureController::worldPosToChunkCoord(glm::fvec3 pos)
{
	return glm::ivec3(
		std::round(pos.x / (m_chunkMeterSizeX + m_half)),
		std::round(pos.y / (m_chunkMeterSizeY + m_half)),
		std::round(pos.z / (m_chunkMeterSizeZ + m_half))
	);
}

glm::fvec3 StructureController::chunkCoordToWorldPos(glm::ivec3 chunk_coord)
{
	return glm::fvec3(
		std::round((chunk_coord.x * m_chunkMeterSizeX) - m_half),
		std::round((chunk_coord.y * m_chunkMeterSizeY) - m_half),
		std::round((chunk_coord.z * m_chunkMeterSizeZ) - m_half)
	);
}

glm::ivec3 StructureController::voxelToChunk(glm::ivec3 location)
{
	int x = floor_to_int(location.x / (float)m_chunk_size_x);
	int y = floor_to_int(location.y / (float)m_chunk_size_y);
	int z = floor_to_int(location.z / (float)m_chunk_size_z);
	return glm::ivec3(x, y, z);
}

glm::ivec3 StructureController::chunkToVoxel(glm::ivec3 location)
{
	int x = ((location.x) * m_chunk_size_x);
	int y = ((location.y) * m_chunk_size_y);
	int z = ((location.z) * m_chunk_size_z);
	return glm::ivec3(x, y, z);
}

glm::ivec3 StructureController::globalToLocalChunkCoord(glm::ivec3 location) {
	glm::ivec3 ChunkCoord = voxelToChunk(location);
	return GlobalToLocalChunkCoord(ChunkCoord, location);
}

glm::ivec3 StructureController::globalToLocalChunkCoord(glm::ivec3 ChunkCoord, glm::ivec3 location) {
	int x = location.x - (ChunkCoord.x * m_chunk_size_x);
	int y = location.y - (ChunkCoord.y * m_chunk_size_y);
	int z = location.z - (ChunkCoord.z * m_chunk_size_z);
	return glm::ivec3(x, y, z);
}

glm::ivec3 StructureController::localToGlobalCoord(glm::ivec3 Chunk, glm::ivec3 location) {
	int x = location.x + (Chunk.x * m_chunk_size_x);
	int y = location.y + (Chunk.y * m_chunk_size_y);
	int z = location.z + (Chunk.z * m_chunk_size_z);
	return glm::ivec3(x, y, z);
}

glm::fvec3 StructureController::voxelToWorld(glm::ivec3 loc)
{
	float newX = (((loc.x / (float)m_voxelsPerMeter) - m_half));
	float newY = (((loc.y / (float)m_voxelsPerMeter) - m_half));
	float newZ = (((loc.z / (float)m_voxelsPerMeter) - m_half));
	return glm::fvec3(newX, newY, newZ);
}

glm::ivec3 StructureController::worldToVoxel(glm::fvec3 worldPos)
{
	int x = floor_to_int(((worldPos.x) + m_half) * (float)m_voxelsPerMeter);
	int y = floor_to_int(((worldPos.y) + m_half) * (float)m_voxelsPerMeter);
	int z = floor_to_int(((worldPos.z) + m_half) * (float)m_voxelsPerMeter);
	return glm::ivec3(x, y, z);
}


