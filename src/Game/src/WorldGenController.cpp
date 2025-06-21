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

void WorldGenController::Submit_Terrain_Modification(glm::ivec3 chunk, TerrainMod value)
{
	TerrainModEntry entry{};
	entry.chunk = chunk;
	entry.changes.push_back(value);
	m_terrain_change_queue.push(entry);
}

void WorldGenController::Submit_Terrain_Modification(glm::ivec3 chunk, std::vector<TerrainMod> values)
{
	TerrainModEntry entry{};
	entry.chunk = chunk;
	entry.changes = values;
	m_terrain_change_queue.push(entry);
}

glm::fvec3 WorldGenController::Target_Position()
{
	return mTarget->Position();
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
		TerrainModEntry entry = m_terrain_change_queue.front();
		m_terrain_change_queue.pop();

		glm::ivec3 chunk = entry.chunk;
		std::vector<TerrainMod> changes = entry.changes;

		if (!Chunk_Exists(chunk)) {
			continue;
		}

		for (const auto& v_change : changes)
		{
			glm::ivec3 voxel_coord = v_change.Voxel;
			voxel_coord.x += m_terrain_mods->Grid_Offset();
			voxel_coord.y += m_terrain_mods->Grid_Offset();
			voxel_coord.z += m_terrain_mods->Grid_Offset();
			m_terrain_mods->Set_Chunk_Data(chunk, voxel_coord, v_change.ISO, v_change.Type);
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


