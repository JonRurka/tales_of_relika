#include "WorldTerrain.h"

#include "Logger.h"
#include "Resources.h"
#include "Game_Resources.h"

#include "ServerTerrainChunk.h"

namespace {
	int floor_to_int(float val) {
		return static_cast<int>(std::floor(val));
	}
}

WorldTerrain::WorldTerrain()
{
}

void WorldTerrain::Init(World* world)
{
	m_world = world;

	std::string block_types_str = Resources::Get_Data_File_String(Game_Resources::Data_Files::BLOCK_TYPES);
	//Logger::LogDebug(LOG_POS("Init"), "%s", block_types_str.c_str());
	
	

	m_half = 0;// ((1 / m_voxelsPerMeter) / 2.0);

	int Max_Verts = (int)Utilities::Vertex_Limit_Mode::Chunk_Max;
	m_vertices = new glm::vec4[Max_Verts];
	//m_normals = new glm::vec4[Max_Verts];
	m_triangles = new unsigned int[Max_Verts];


	compute_triangles();
	create_chunk_cache();
	initialize_voxel_engine();

	Logger::LogInfo(LOG_POS("Init"), "Initialized");
	
}

void WorldTerrain::Update(float dt)
{
	for (const auto& pair : m_chunk_map)
	{
		pair.second.chunk_comp->Update(dt);
	}


	process_deletions();
	process_additions();
	process_modifications();
}

void WorldTerrain::Refresh_Chunk(glm::ivec3 chunk)
{
	TerrainMod val(glm::vec3(0, 0, 0));
	Modify_Voxel(chunk, val, false);
}

void WorldTerrain::Modify_Voxel_ISO(glm::ivec3 voxel, float iso)
{
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

void WorldTerrain::Modify_Voxel_Type(glm::ivec3 voxel, int type)
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

void WorldTerrain::Modify_Voxel(std::vector<TerrainMod> values)
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

void WorldTerrain::Modify_Voxel(glm::ivec3 chunk, TerrainMod value, bool update_neighbor)
{
	std::vector<TerrainMod> values;
	values.push_back(value);
	Modify_Voxel(chunk, values, update_neighbor);
}

void WorldTerrain::Modify_Voxel(glm::ivec3 chunk, std::vector<TerrainMod> values, bool update_neighbor)
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

void WorldTerrain::Submit_Terrain_Modification(glm::ivec3 chunk, TerrainMod value)
{
	TerrainModEntry entry{};
	entry.chunk = chunk;
	entry.changes.push_back(value);
	m_terrain_change_queue.push(entry);
}

void WorldTerrain::Submit_Terrain_Modification(glm::ivec3 chunk, std::vector<TerrainMod> values)
{
	TerrainModEntry entry{};
	entry.chunk = chunk;
	entry.changes = values;
	m_terrain_change_queue.push(entry);
}

int WorldTerrain::Hash_Chunk(glm::ivec3 chunk) 
{
	return Utilities::Hash_Chunk_Coord(chunk);
}

ServerTerrainChunk* WorldTerrain::Get_Chunk(glm::ivec3 chunk_coord)
{
	return get_chunk(chunk_coord).chunk_comp;
}

std::vector<glm::ivec3> WorldTerrain::Get_Chunk_Coords(glm::fvec3 loc, int radius, int depth)
{
	std::vector<glm::ivec3> res;

	glm::ivec3 chunk_center = WorldPosToChunkCoord(loc);
	std::vector<glm::ivec3> cols = get_columns_in_radius(chunk_center.x, chunk_center.z, radius);
	int y_start = chunk_center.y - (depth / 2);

	for (const auto& c : cols) {
		for (int i = 0; i < depth; i++) {
			int y = y_start + i;
			glm::ivec3 chunk_coord = glm::ivec3(c.x, y, c.y);
			res.push_back(chunk_coord);
		}
	}

	return res;
}

WorldTerrain::ChunkRef WorldTerrain::get_chunk(glm::ivec3 chunk_coord)
{
	int hash = Utilities::Hash_Chunk_Coord(chunk_coord.x, chunk_coord.y, chunk_coord.z);
	return m_chunk_map[hash];
}

void WorldTerrain::initialize_voxel_engine()
{
	settings.GetSettings()->setString("programDir", "");
	settings.GetSettings()->setFloat("voxelsPerMeter", m_voxelsPerMeter);
	settings.GetSettings()->setInt("chunkMeterSizeX", m_chunkMeterSizeX);
	settings.GetSettings()->setInt("chunkMeterSizeY", m_chunkMeterSizeX);
	settings.GetSettings()->setInt("chunkMeterSizeZ", m_chunkMeterSizeX);
	settings.GetSettings()->setInt("TotalBatchGroups", 1);
	settings.GetSettings()->setInt("BatchesPerGroup", 1);
	settings.GetSettings()->setInt("InvertTrianges", false);
	settings.GetSettings()->setBool("SharedGL", false);

	m_chunk_size_x = m_chunkMeterSizeX * m_voxelsPerMeter;
	m_chunk_size_y = m_chunkMeterSizeY * m_voxelsPerMeter;
	m_chunk_size_z = m_chunkMeterSizeZ * m_voxelsPerMeter;

	m_builder = new SmoothVoxelBuilder();
	m_builder->Init(&settings);

	m_terrain_mods = ((SmoothVoxelBuilder*)m_builder)->Get_Terrain_Modifications();
	m_heightmap_gen = ((SmoothVoxelBuilder*)m_builder)->Get_Heightmap_Generator();

	Logger::LogInfo(LOG_POS("initialize_voxel_engine"), "Initialized Voxel Engine.");
}

void WorldTerrain::process_additions()
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
}

void WorldTerrain::process_deletions()
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

bool WorldTerrain::process_batch()
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

	//glm::dvec4 gen_times = m_builder->Generate(&gen_options);
	//glm::dvec4 render_times = m_builder->Render(&render_options);

	std::vector<glm::ivec4> counts = {glm::ivec4(1, 0, 0, 0)}; //m_builder->GetSize();

	int num_verts = counts[0].x;

	/*m_builder->Extract(
		m_vertices,
		nullptr,
		nullptr,
		nullptr,
		counts[0]
	);*/

	std::vector<glm::vec4> verts(m_vertices, m_vertices + counts[0].x);
	std::vector<unsigned int> tris(m_triangles, m_triangles + counts[0].x);
	//std::vector<glm::vec4> normals(m_normals, m_normals + counts[0].x);

	batch[0].chunk_comp->Process_Mesh_Update(verts, tris, counts[0]);

	double end_time = Utilities::Get_Time();
	double duration_ms = (end_time - start_time) * 1000.0;


}

void WorldTerrain::process_modifications()
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

		m_builder->Extract(
			m_vertices,
			nullptr,
			nullptr,
			nullptr,
			counts[0]
		);
		std::vector<glm::vec4> verts(m_vertices, m_vertices + counts[0].x);
		std::vector<unsigned int> tris(m_triangles, m_triangles + counts[0].x);

		get_chunk(chunk).chunk_comp->Process_Mesh_Update(verts, tris, counts[0]);
	}
}

void WorldTerrain::create_chunk_cache() 
{
	double start = Utilities::Get_Time();

	for (int i = 0; i < m_initial_cached_columns; ++i) {
		ChunkRef chk = create_chunk_object();
		m_cached_chunks.push(chk);
	}

	double end = Utilities::Get_Time();
	Logger::LogDebug(LOG_POS("create_chunk_cache"), "Created %i cached chunks in %f ms.", m_initial_cached_columns, (end - start) * 1000);
}

WorldTerrain::ChunkRef WorldTerrain::create_chunk_object()
{
	ServerTerrainChunk* comp = new ServerTerrainChunk();
	comp->Init(this);

	ChunkRef chk{};
	chk.chunk_comp = comp;

	return chk;
}

ServerTerrainChunk* WorldTerrain::queue_chunk_create(glm::ivec3 chunk_coord)
{
	int hash = Utilities::Hash_Chunk_Coord(chunk_coord.x, chunk_coord.y, chunk_coord.z);

	if (m_chunk_map.contains(hash)) {
		return m_chunk_map[hash].chunk_comp;
	}

	ChunkRef chunk = m_cached_chunks.front();
	m_cached_chunks.pop();

	chunk.chunk_comp->Assign(chunk_coord);
	chunk.chunk_coord = chunk_coord;

	m_chunk_map[hash] = chunk;

	m_create_queue.push(chunk);

	//Logger::LogDebug(LOG_POS("queue_chunk_create"), "created chunk (%i): (%i, %i, %i)",
	//	m_cached_chunks.size(), chunk_coord.x, chunk_coord.y, chunk_coord.z);

	return chunk.chunk_comp;
}

void WorldTerrain::queue_chunk_delete(glm::ivec3 chunk_coord) 
{
	m_delete_queue.push(chunk_coord);
}

bool WorldTerrain::chunk_exists(glm::ivec3 chunk_coord) 
{
	int hash = Utilities::Hash_Chunk_Coord(chunk_coord.x, chunk_coord.y, chunk_coord.z);
	return m_chunk_map.contains(hash);
}

void WorldTerrain::remove_chunk(glm::ivec3 chunk_coord) 
{
	int hash = Utilities::Hash_Chunk_Coord(chunk_coord.x, chunk_coord.y, chunk_coord.z);
	m_chunk_map.erase(hash);
}

void WorldTerrain::compute_triangles()
{
	int Max_Verts = (int)Utilities::Vertex_Limit_Mode::Chunk_Max;
	bool m_invert_tris = false;
	for (int i = 0; i < Max_Verts; i += 3) {
		unsigned int tris_start = i;

		if (m_invert_tris) {
			m_triangles[tris_start + 0] = tris_start + 0;
			m_triangles[tris_start + 1] = tris_start + 1;
			m_triangles[tris_start + 2] = tris_start + 2;
		}
		else
		{
			m_triangles[tris_start + 0] = tris_start + 2;
			m_triangles[tris_start + 1] = tris_start + 1;
			m_triangles[tris_start + 2] = tris_start + 0;
		}
	}
	Logger::LogDebug(LOG_POS("compute_triangles"), "Computed Trianges.");
}

std::vector<glm::ivec3> WorldTerrain::get_columns_in_radius(int center_x, int center_z, int radius)
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

glm::ivec3 WorldTerrain::worldPosToChunkCoord(glm::fvec3 pos)
{
	return glm::ivec3(
		std::round(pos.x / (m_chunkMeterSizeX + m_half)),
		std::round(pos.y / (m_chunkMeterSizeY + m_half)),
		std::round(pos.z / (m_chunkMeterSizeZ + m_half))
	);
}

glm::fvec3 WorldTerrain::chunkCoordToWorldPos(glm::ivec3 chunk_coord)
{
	return glm::fvec3(
		std::round((chunk_coord.x * m_chunkMeterSizeX) - m_half),
		std::round((chunk_coord.y * m_chunkMeterSizeY) - m_half),
		std::round((chunk_coord.z * m_chunkMeterSizeZ) - m_half)
	);
}

glm::ivec3 WorldTerrain::voxelToChunk(glm::ivec3 location)
{
	int x = floor_to_int(location.x / (float)m_chunk_size_x);
	int y = floor_to_int(location.y / (float)m_chunk_size_y);
	int z = floor_to_int(location.z / (float)m_chunk_size_z);
	return glm::ivec3(x, y, z);
}

glm::ivec3 WorldTerrain::chunkToVoxel(glm::ivec3 location)
{
	int x = ((location.x) * m_chunk_size_x);
	int y = ((location.y) * m_chunk_size_y);
	int z = ((location.z) * m_chunk_size_z);
	return glm::ivec3(x, y, z);
}

glm::ivec3 WorldTerrain::globalToLocalChunkCoord(glm::ivec3 location) {
	glm::ivec3 ChunkCoord = voxelToChunk(location);
	return GlobalToLocalChunkCoord(ChunkCoord, location);
}

glm::ivec3 WorldTerrain::globalToLocalChunkCoord(glm::ivec3 ChunkCoord, glm::ivec3 location) {
	int x = location.x - (ChunkCoord.x * m_chunk_size_x);
	int y = location.y - (ChunkCoord.y * m_chunk_size_y);
	int z = location.z - (ChunkCoord.z * m_chunk_size_z);
	return glm::ivec3(x, y, z);
}

glm::ivec3 WorldTerrain::localToGlobalCoord(glm::ivec3 Chunk, glm::ivec3 location) {
	int x = location.x + (Chunk.x * m_chunk_size_x);
	int y = location.y + (Chunk.y * m_chunk_size_y);
	int z = location.z + (Chunk.z * m_chunk_size_z);
	return glm::ivec3(x, y, z);
}

glm::fvec3 WorldTerrain::voxelToWorld(glm::ivec3 loc)
{
	float newX = (((loc.x / (float)m_voxelsPerMeter) - m_half));
	float newY = (((loc.y / (float)m_voxelsPerMeter) - m_half));
	float newZ = (((loc.z / (float)m_voxelsPerMeter) - m_half));
	return glm::fvec3(newX, newY, newZ);
}

glm::ivec3 WorldTerrain::worldToVoxel(glm::fvec3 worldPos)
{
	int x = floor_to_int(((worldPos.x) + m_half) * (float)m_voxelsPerMeter);
	int y = floor_to_int(((worldPos.y) + m_half) * (float)m_voxelsPerMeter);
	int z = floor_to_int(((worldPos.z) + m_half) * (float)m_voxelsPerMeter);
	return glm::ivec3(x, y, z);
}
