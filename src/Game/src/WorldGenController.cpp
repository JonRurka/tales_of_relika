#include "WorldGenController.h"

#include "Stitch_VBO.h"
#include "Opaque_Chunk_Material.h"
#include "TerrainChunk.h"
#include "Game_Resources.h"
#include "Material_Types.h"

#include <algorithm>

WorldGenController* WorldGenController::m_Instance{nullptr};

namespace {
	std::vector<double> construct_times;
	const int max_times = 1000;


	int floor_to_int(float val) {
		return static_cast<int>(std::floor(val));
	}
}

int WorldGenController::Hash_Chunk(glm::ivec3 chunk)
{
	return Utilities::Hash_Chunk_Coord(chunk);
}

void WorldGenController::Init()
{
	m_Instance = this;

	std::string block_types_str = Resources::Get_Data_File_String(Game_Resources::Data_Files::BLOCK_TYPES);
	Logger::LogDebug(LOG_POS("Init"), "%s", block_types_str.c_str());


	Logger::LogInfo(LOG_POS("Init"), "Initialized");

	mTarget = Object()->Get_Transform(); // TODO: Get player transform.

	m_half = 0;// ((1 / m_voxelsPerMeter) / 2.0);

	int padded_radius = m_max_chunk_radius + 2;
	m_max_column_target = std::round(3.14 * (m_max_chunk_radius * m_max_chunk_radius));
	m_max_cached_columns = std::round(3.14 * (padded_radius * padded_radius));

	m_max_cached_chunks = m_max_cached_columns * m_chunks_depth;


	m_chunk_opaque_mat = new Opaque_Chunk_Material(); 
	//m_chunk_opaque_mat->setTexture("diffuse", m_diffuse_texture_array);
	m_chunk_opaque_mat->setTexture("diffuse", Material_Types::Instance()->Diffuse_Texture_Array());
	m_chunk_opaque_mat->setTexture("normal_maps", Material_Types::Instance()->Normal_Texture_Array());
	m_chunk_opaque_mat->SetVec3("material.ambientColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_chunk_opaque_mat->SetVec3("material.diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_chunk_opaque_mat->SetVec2("material.scale", glm::vec2(1.0f, 1.0f));
	m_chunk_opaque_mat->setFloat("material.shininess", 8.0f);
	m_chunk_opaque_mat->setFloat("material.specular_intensity", 1.0f);
	m_chunk_opaque_mat->SetVec3("globalAmbientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_chunk_opaque_mat->setFloat("globalAmbientIntensity", 0.01f);

	//Logger::LogDebug(LOG_POS("Init"), "Max cached chunks: %i", m_max_cached_chunks);

	initialize_voxel_engine();

	Start();
}

void WorldGenController::Update(float dt)
{
	process_deletions();
	process_additions();

	if (m_gen_finished) {
		process_modifications();
	}
}

void WorldGenController::Start()
{
	create_chunk_cache();
	generate_circular();

	//Logger::LogDebug(LOG_POS("Start"), "The queue is %i long", m_create_queue.size());
}

TerrainChunk* WorldGenController::Get_Chunk(glm::ivec3 chunk_coord)
{
	return get_chunk(chunk_coord).chunk_comp;
}

ISO_Sampler* WorldGenController::Get_ISO_Sampler()
{
	return ((SmoothVoxelBuilder*)m_builder)->Get_ISO_Sampler();
}

void WorldGenController::Refresh_Chunk(glm::ivec3 chunk)
{
	//Logger::LogDebug(LOG_POS("Refresh_Chunk"), "Refresh chunk (%i, %i, %i)",
	//	chunk.x, chunk.y, chunk.z);
	TerrainMod val(glm::vec3(0, 0, 0));
	Modify_Voxel(chunk, val, false);
}

void WorldGenController::Modify_Voxel_ISO(glm::ivec3 voxel, float iso)
{
	//Logger::LogDebug(LOG_POS("Modify_Voxel_ISO"), "Modify voxel");
	glm::ivec3 chunk = voxelToChunk(voxel);
	glm::ivec3 voxel_local = globalToLocalChunkCoord(chunk, voxel);

	if (!Chunk_Exists(chunk)) {
		//int tmp = Utilities::Hash_Chunk_Coord(chunk.x, chunk.y, chunk.z);
		//Logger::LogDebug(LOG_POS("Modify_Voxel_ISO"), "We can NOT modify the voxel for chunk (%i, %i, %i): %i",
		//	chunk.x, chunk.y, chunk.z);
		return;
	}
	//Logger::LogDebug(LOG_POS("Modify_Voxel_ISO"), "We can modify the voxel");

	get_chunk(chunk).chunk_comp->VoxelChanged(voxel_local, true, iso, false, 0);

	TerrainMod mod(voxel_local, iso);
	Modify_Voxel(chunk, mod);
}

void WorldGenController::Modify_Voxel_Type(glm::ivec3 voxel, int type)
{
	glm::ivec3 chunk = voxelToChunk(voxel);
	glm::ivec3 voxel_local = globalToLocalChunkCoord(chunk, voxel);

	if (!Chunk_Exists(chunk)) {
		return;
	}

	get_chunk(chunk).chunk_comp->VoxelChanged(voxel_local, false, 0, true, type);

	TerrainMod mod(voxel_local, type);
	Modify_Voxel(chunk, mod);
}

void WorldGenController::Modify_Voxel(std::vector<TerrainMod> values)
{
	std::unordered_map<int, std::vector<TerrainMod>> mods;

	for (const auto& elem : values) 
	{
		glm::ivec3 chunk = voxelToChunk(elem.Voxel);
		glm::ivec3 voxel_local = globalToLocalChunkCoord(chunk, elem.Voxel);

		//int chunk_hash = 
		int chunk_hash = Utilities::Hash_Chunk_Coord(chunk.x, chunk.y, chunk.z);
		if (!mods.contains(chunk_hash))
		{
			mods[chunk_hash] = std::vector<TerrainMod>();
		}

		// TODO: call terrain chunk event

		TerrainMod mod(voxel_local);
		mod.Change_ISO = elem.Change_ISO;
		mod.Change_Type = elem.Change_Type;
		mod.ISO = elem.ISO;
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

void WorldGenController::Modify_Voxel(glm::ivec3 chunk, TerrainMod value, bool update_neighbor)
{
	std::vector<TerrainMod> values;
	values.push_back(value);
	Modify_Voxel(chunk, values, update_neighbor);
}

void WorldGenController::Modify_Voxel(glm::ivec3 chunk, std::vector<TerrainMod> values, bool update_neighbor)
{

	Submit_Terrain_Modification(chunk, values);

	
	if (update_neighbor) {
		std::unordered_map<int, std::vector<TerrainMod>> neighboor_updates;
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

			TerrainMod other_mod(other_chunk_voxel);
			other_mod.ISO = val.ISO;
			other_mod.Change_ISO = val.Change_ISO;
			other_mod.Type = val.Type;
			other_mod.Change_Type = val.Change_Type;
			other_mod.chunk_coord = other_chunk_voxel;

			int other_hash = Utilities::Hash_Chunk_Coord(other_chunk.x, other_chunk.y, other_chunk.z);
			if (!neighboor_updates.contains(other_hash))
			{
				neighboor_updates[other_hash] = std::vector<TerrainMod>();
			}

			neighboor_updates[other_hash].push_back(other_mod);
		}

		for (const auto& pair : neighboor_updates) {
			glm::ivec3 other_chunk = pair.second[0].chunk_coord;
			Submit_Terrain_Modification(other_chunk, pair.second);
		}

	}

}

void WorldGenController::Submit_Terrain_Modification(glm::ivec3 chunk, TerrainMod value)
{
	if (!m_gen_finished)
		return;

	TerrainModEntry entry{};
	entry.chunk = chunk;
	entry.changes.push_back(value);
	m_terrain_change_queue.push(entry);
	//Logger::LogDebug(LOG_POS("Submit_Terrain_Modification"), "Submitted terrain change for chunk (%i, %i, %i).",
	//	chunk.x, chunk.y, chunk.z);
}

void WorldGenController::Submit_Terrain_Modification(glm::ivec3 chunk, std::vector<TerrainMod> values)
{
	if (!m_gen_finished)
		return;

	TerrainModEntry entry{};
	entry.chunk = chunk;
	entry.changes = values;
	m_terrain_change_queue.push(entry);
	//Logger::LogDebug(LOG_POS("Submit_Terrain_Modification_arr"), "Submitted terrain %i changes for chunk (%i, %i, %i).",
	//	values.size(), chunk.x, chunk.y, chunk.z);
}

glm::fvec3 WorldGenController::Target_Position()
{
	return mTarget->Position();
}

glm::ivec3 WorldGenController::Target_Chunk()
{
	glm::vec3 target_pos = Target_Position();
	glm::ivec3 target_chunk = WorldGenController::WorldPosToChunkCoord(target_pos);
	return target_chunk;
}

WorldGenController::ChunkRef WorldGenController::get_chunk(glm::ivec3 chunk_coord)
{
	int hash = Utilities::Hash_Chunk_Coord(chunk_coord.x, chunk_coord.y, chunk_coord.z);
	return m_chunk_map[hash];
}

void WorldGenController::initialize_voxel_engine()
{
	settings.GetSettings()->setString("programDir", "");
	settings.GetSettings()->setFloat("voxelsPerMeter", m_voxelsPerMeter);
	settings.GetSettings()->setInt("chunkMeterSizeX", m_chunkMeterSizeX);
	settings.GetSettings()->setInt("chunkMeterSizeY", m_chunkMeterSizeX);
	settings.GetSettings()->setInt("chunkMeterSizeZ", m_chunkMeterSizeX);
	settings.GetSettings()->setInt("TotalBatchGroups", 1);
	settings.GetSettings()->setInt("BatchesPerGroup", 4);
	settings.GetSettings()->setInt("InvertTrianges", false);

	m_chunk_size_x = m_chunkMeterSizeX * m_voxelsPerMeter;
	m_chunk_size_y = m_chunkMeterSizeY * m_voxelsPerMeter;
	m_chunk_size_z = m_chunkMeterSizeZ * m_voxelsPerMeter;

	m_builder = new SmoothVoxelBuilder();
	m_builder->Init(&settings);

	m_terrain_mods = ((SmoothVoxelBuilder*)m_builder)->Get_Terrain_Modifications();

	int max_vert = (int)Utilities::Vertex_Limit_Mode::Chunk_Max;

	vbo_stitch = new Stitch_VBO();
	vbo_stitch->Init(m_builder, max_vert);
}

void WorldGenController::process_additions()
{
	bool has_items = !m_create_queue.empty();

	if (!has_items)
		return;

	double start = Utilities::Get_Time();
	double end = start;
	double timer = (end - start);

	while (has_items && timer < m_process_time_ms) {
		has_items = process_batch();

		end = Utilities::Get_Time();
		timer = (end - start) * 1000.0;
	}


	if (m_world_gen_started && !m_gen_finished)
	{

		if (m_create_queue.empty()) {

			double sum = 0;
			for (const auto& elem : construct_times) {
				sum += elem;
			}
			double avg = sum / construct_times.size();

			m_gen_stop = Utilities::Get_Time();
			m_gen_finished = true;
			double m_total_time = (m_gen_stop - m_gen_start) * 1000.0f;
			//Logger::LogInfo(LOG_POS("process_additions"), "%i Chunks generated in %f ms. Average construct time: %f ms.", 
			//	m_num_filled_init_chunks, (m_gen_stop - m_gen_start) * 1000.0f, avg * 1000.0f);
			double gen_average_ms = m_chunk_init_all_gen_time_ms / m_num_all_init_chunks;
			Logger::LogInfo(LOG_POS("process_additions"), "%i Chunks generated in %f ms. Average chunk gen time: %f ms. (%f chunks/sec). Total time: %f ms",
				m_num_all_init_chunks, m_chunk_init_all_gen_time_ms, gen_average_ms, 1.0 / (gen_average_ms / 1000.0), m_total_time);

			//Logger::LogDebug(LOG_POS("process_additions"), "Num chunks in map: %i", m_chunk_map.size());
		}

		// TODO: Other stuff when chunk gen has finished.

	}

}

void WorldGenController::process_deletions()
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

// return true if more items in queue.
bool WorldGenController::process_batch()
{
	ChunkRef batch[MAX_BATCH_SIZE];

	ChunkGenerationOptions gen_options;
	ChunkRenderOptions render_options;

	int num_additions = 0;
	while (!m_create_queue.empty() && num_additions < m_batch_size) {
		ChunkRef ref = m_create_queue.front();
		m_create_queue.pop();
		batch[num_additions] = ref;
		num_additions++;

		glm::ivec4 chunk_loc = glm::ivec4(ref.chunk_coord, 0);
		gen_options.locations.push_back(chunk_loc);
		render_options.locations.push_back(chunk_loc);

	}

	if (num_additions <= 0) {
		return false;
	}

	double start_time = Utilities::Get_Time();

	glm::dvec4 gen_times = m_builder->Generate(&gen_options);
	glm::dvec4 render_times = m_builder->Render(&render_options);

	std::vector<glm::ivec4> counts = m_builder->GetSize();

	bool has_full_chunks = false;
	for (int i = 0; i < num_additions; i++) {
		int num_verts = counts[i].x;
		batch[i].chunk_comp->Process_Mesh_Update(counts[i]);
		if (num_verts > 0) {
			m_num_filled_init_chunks++;
			has_full_chunks = true;
		}
		m_num_all_init_chunks++;
	}

	double end_time = Utilities::Get_Time();
	double duration_ms = (end_time - start_time) * 1000.0;
	m_chunk_init_all_gen_time_ms += duration_ms;

	if (has_full_chunks) { // chunks where nothing was generated don't count.
		construct_times.push_back(gen_times.y);
		if (construct_times.size() >= max_times) {
			construct_times.erase(construct_times.begin());
		}
	}

	return !m_create_queue.empty();
}

void WorldGenController::process_modifications()
{

	while (!m_terrain_change_queue.empty()) 
	{
		//Logger::LogDebug(LOG_POS("process_modifications"), "Items in mod queue: %i", m_terrain_change_queue.size());

		TerrainModEntry entry = m_terrain_change_queue.front();
		m_terrain_change_queue.pop();

		glm::ivec3 chunk = entry.chunk;
		std::vector<TerrainMod> changes = entry.changes;

		if (!Chunk_Exists(chunk)) {
			continue;
		}

		for (const auto& v_change : changes)
		{
			if (v_change.Change_ISO || v_change.Change_Type) {
				glm::ivec3 voxel_coord = v_change.Voxel;
				voxel_coord.x += m_terrain_mods->Grid_Offset();
				voxel_coord.y += m_terrain_mods->Grid_Offset();
				voxel_coord.z += m_terrain_mods->Grid_Offset();
				m_terrain_mods->Set_Chunk_Data(chunk, voxel_coord, v_change.Change_ISO, v_change.ISO, v_change.Change_Type, v_change.Type);
			}
		}


		ChunkGenerationOptions gen_options;
		ChunkRenderOptions render_options;

		glm::ivec4 chunk_loc = glm::ivec4(chunk, 0);
		gen_options.locations.push_back(chunk_loc);
		render_options.locations.push_back(chunk_loc);

		glm::dvec4 gen_times = m_builder->Generate(&gen_options);
		glm::dvec4 render_times = m_builder->Render(&render_options);
		std::vector<glm::ivec4> counts = m_builder->GetSize();

		get_chunk(chunk).chunk_comp->Process_Mesh_Update(counts[0]);
	}
}

WorldGenController::ChunkRef WorldGenController::create_chunk_object()
{
	WorldObject* obj = Instantiate("Cached Voxel Chunk");
	obj->Get_Transform()->Set_Verbos(false);
	obj->Get_Transform()->Position(glm::vec3(0.0, 1000.0, 0.0));
	TerrainChunk* comp = obj->Add_Component<TerrainChunk>();
	comp->Init(this, vbo_stitch);

	ChunkRef chk{};
	chk.chunk_obj = obj;
	chk.chunk_comp = comp;
	//m_chunk_map[hash] = chk;

	return chk;
}

// Way to multi-thread this.
// https://stackoverflow.com/questions/61857306/safely-skip-an-task-if-mutex-is-locked
void WorldGenController::generate_circular()
{
	double start = Utilities::Get_Time();

	if (mTarget == nullptr) {
		Logger::LogError(LOG_POS("generate_circular"), "Cannot generate chunks with null target.");
		return;
	}


	glm::ivec3 target_chunk_pos = worldPosToChunkCoord(mTarget->Position());

	std::vector<glm::ivec3> cols = get_columns_in_radius(target_chunk_pos.x, target_chunk_pos.z, m_max_chunk_radius);

	//Logger::LogInfo(LOG_POS("generate_circular"), "Generating %i Columns", cols.size());

	int y_start = target_chunk_pos.y - (m_chunks_depth / 2);

	int queued_chunks_num = 0;

	for (const auto& c : cols) {
		for (int i = 0; i < m_chunks_depth; i++) {
			int y = y_start + i;
			glm::ivec3 chunk_coord = glm::ivec3(c.x, y, c.y);
			bool queued = queue_chunk_create(chunk_coord);
			if (queued) {
				queued_chunks_num++;
			}
		}
	}

	double end = Utilities::Get_Time();
	Logger::LogDebug(LOG_POS("generate_circular"), "Queued %i chunks in %f ms.", queued_chunks_num, (end - start) * 1000);

	m_gen_start = Utilities::Get_Time();
	m_world_gen_started = true;
}

void WorldGenController::create_chunk_cache()
{
	double start = Utilities::Get_Time();

	for (int i = 0; i < m_max_cached_chunks; ++i) {
		ChunkRef chk = create_chunk_object();
		m_cached_chunks.push(chk);
	}

	double end = Utilities::Get_Time();
	Logger::LogDebug(LOG_POS("create_chunk_cache"), "Created %i cached chunks in %f ms.", m_max_cached_chunks, (end - start) * 1000);
}

bool WorldGenController::queue_chunk_create(glm::ivec3 chunk_coord)
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

	m_create_queue.push(chunk);

	return true;
}

void WorldGenController::queue_chunk_delete(glm::ivec3 chunk_coord)
{
	m_delete_queue.push(chunk_coord);
}

bool WorldGenController::chunk_exists(glm::ivec3 chunk_coord)
{
	int hash = Utilities::Hash_Chunk_Coord(chunk_coord.x, chunk_coord.y, chunk_coord.z);
	return m_chunk_map.contains(hash);
}

void WorldGenController::remove_chunk(glm::ivec3 chunk_coord)
{
	int hash = Utilities::Hash_Chunk_Coord(chunk_coord.x, chunk_coord.y, chunk_coord.z);
	m_chunk_map.erase(hash);
}

std::vector<glm::ivec3> WorldGenController::get_columns_in_radius(int center_x, int center_z, int radius)
{
	const int padding = 1;
	int padded_rad = (radius + padding);

	int x_start = center_x - padded_rad;
	int z_start = center_z - padded_rad;
	glm::fvec2 center = glm::fvec2(center_x, center_z);

	std::vector<glm::ivec3> col_list;
	col_list.reserve(radius * radius);

	for (int x = 0; x < (padded_rad * 2); ++x) {
		for (int z = 0; z < (padded_rad * 2); ++z) {
			//break; // TODO

			int chunk_x = x_start + x;
			int chunk_z = z_start + z;

			glm::fvec2 col_coords = glm::fvec2(chunk_x, chunk_z);
			float dist = glm::distance(center, col_coords);

			if (dist <= radius) {
				glm::ivec3 col_dat = glm::ivec3(chunk_x, chunk_z, dist);
				col_list.push_back(col_dat);
			}
		}
	}
	//col_list.push_back(glm::ivec3(0, 0, 0));

	std::ranges::sort(col_list, std::ranges::less{}, &glm::ivec3::z);

	return col_list;

}

glm::ivec3 WorldGenController::worldPosToChunkCoord(glm::fvec3 pos)
{
	return glm::ivec3(
		std::round(pos.x / (m_chunkMeterSizeX + m_half)), 
		std::round(pos.y / (m_chunkMeterSizeY + m_half)),
		std::round(pos.z / (m_chunkMeterSizeZ + m_half))
	);
}

glm::fvec3 WorldGenController::chunkCoordToWorldPos(glm::ivec3 chunk_coord)
{
	return glm::fvec3(
		std::round((chunk_coord.x * m_chunkMeterSizeX) - m_half),
		std::round((chunk_coord.y * m_chunkMeterSizeY) - m_half),
		std::round((chunk_coord.z * m_chunkMeterSizeZ) - m_half)
	);
}

glm::ivec3 WorldGenController::voxelToChunk(glm::ivec3 location)
{
	int x = floor_to_int(location.x / (float)m_chunk_size_x);
	int y = floor_to_int(location.y / (float)m_chunk_size_y);
	int z = floor_to_int(location.z / (float)m_chunk_size_z);
	return glm::ivec3(x, y, z);
}

glm::ivec3 WorldGenController::chunkToVoxel(glm::ivec3 location)
{
	int x = ((location.x) * m_chunk_size_x);
	int y = ((location.y) * m_chunk_size_y);
	int z = ((location.z) * m_chunk_size_z);
	return glm::ivec3(x, y, z);
}

glm::ivec3 WorldGenController::globalToLocalChunkCoord(glm::ivec3 location) {
	glm::ivec3 ChunkCoord = voxelToChunk(location);
	return GlobalToLocalChunkCoord(ChunkCoord, location);
}

glm::ivec3 WorldGenController::globalToLocalChunkCoord(glm::ivec3 ChunkCoord, glm::ivec3 location) {
	int x = location.x - (ChunkCoord.x * m_chunk_size_x);
	int y = location.y - (ChunkCoord.y * m_chunk_size_y);
	int z = location.z - (ChunkCoord.z * m_chunk_size_z);
	return glm::ivec3(x, y, z);
}

glm::ivec3 WorldGenController::localToGlobalCoord(glm::ivec3 Chunk, glm::ivec3 location) {
	int x = location.x + (Chunk.x * m_chunk_size_x);
	int y = location.y + (Chunk.y * m_chunk_size_y);
	int z = location.z + (Chunk.z * m_chunk_size_z);
	return glm::ivec3(x, y, z);
}

glm::fvec3 WorldGenController::voxelToWorld(glm::ivec3 loc)
{
	float newX = (((loc.x / (float)m_voxelsPerMeter) - m_half));
	float newY = (((loc.y / (float)m_voxelsPerMeter) - m_half));
	float newZ = (((loc.z / (float)m_voxelsPerMeter) - m_half));
	return glm::fvec3(newX, newY, newZ);
}

glm::ivec3 WorldGenController::worldToVoxel(glm::fvec3 worldPos)
{
	int x = floor_to_int(((worldPos.x) + m_half) * (float)m_voxelsPerMeter);
	int y = floor_to_int(((worldPos.y) + m_half) * (float)m_voxelsPerMeter);
	int z = floor_to_int(((worldPos.z) + m_half) * (float)m_voxelsPerMeter);
	return glm::ivec3(x, y, z);
}

