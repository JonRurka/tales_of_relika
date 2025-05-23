#include "WorldGenController.h"

#include "Stitch_VBO.h"
#include "Opaque_Chunk_Material.h"
#include "TerrainChunk.h"

#include <algorithm>

void WorldGenController::Init()
{
	Logger::LogInfo(LOG_POS("Init"), "Initialized");

	mTarget = Object()->Get_Transform(); // TODO: Get player transform.

	m_half = ((1 / m_voxelsPerMeter) / 2.0);

	int padded_radius = m_max_chunk_radius + 2;
	m_max_column_target = std::round(3.14 * (m_max_chunk_radius * m_max_chunk_radius));
	m_max_cached_columns = std::round(3.14 * (padded_radius * padded_radius));

	m_max_cached_chunks = m_max_cached_columns * m_chunks_depth;

	m_chunk_opaque_mat = new Opaque_Chunk_Material();
	m_chunk_opaque_mat->SetVec3("material.ambientColor", glm::vec3(1.0f, 0.5f, 0.31f));
	m_chunk_opaque_mat->SetVec3("material.diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_chunk_opaque_mat->SetVec2("material.scale", glm::vec2(1.0f, 1.0f));
	m_chunk_opaque_mat->setFloat("material.shininess", 32.0f);
	m_chunk_opaque_mat->setFloat("material.specular_intensity", 1.0f);
	m_chunk_opaque_mat->SetVec3("globalAmbientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_chunk_opaque_mat->setFloat("globalAmbientIntensity", 0.1f);

	//Logger::LogDebug(LOG_POS("Init"), "Max cached chunks: %i", m_max_cached_chunks);

	Start();
}

void WorldGenController::Update(float dt)
{
	


}

void WorldGenController::Start()
{
	create_chunk_cache();
	generate_circular();
}

WorldGenController::ChunkRef WorldGenController::create_chunk_object()
{
	/*int hash = Utilities::Hash_Chunk_Coord(x, y, z);

	if (m_chunk_map.contains(hash)) {
		return m_chunk_map[hash].chunk_obj;
	}*/

	int max_vert = (int)Utilities::Vertex_Limit_Mode::Chunk_Max;
	Mesh* voxel_mesh_test = new Mesh(max_vert * Stitch_VBO::Stride());



	//WorldObject* obj = Instantiate("Voxel Chunk (" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")");
	WorldObject* obj = Instantiate("Cached Voxel Chunk");
	obj->Get_Transform()->Set_Verbos(false);
	obj->Get_MeshRenderer()->Set_Mesh(voxel_mesh_test);
	//obj->Get_MeshRenderer()->Transparent(true);
	//obj->Get_MeshRenderer()->Set_Shader(m_shader);
	obj->Get_Transform()->Translate(0.0, 1000.0, 0.0);
	obj->Get_MeshRenderer()->Set_Material(m_chunk_opaque_mat);
	//obj->Add_Component<MeshCollider>();
	TerrainChunk* comp = obj->Add_Component<TerrainChunk>();

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

	int y_start = 0;

	int queued_chunks_num = 0;

	for (const auto& c : cols) {
		for (int i = 0; i < m_chunks_depth; i++) {
			int y = y_start + i;
			glm::ivec3 chunk_coord = glm::ivec3(c.x, y, c.z);
			bool queued = queue_chunk_create(chunk_coord);

			if (queued) {
				queued_chunks_num++;
			}
		}
	}

	double end = Utilities::Get_Time();;
	Logger::LogDebug(LOG_POS("generate_circular"), "Queued %i chunks in %f ms.", queued_chunks_num, (end - start) * 1000);
}

void WorldGenController::cull_circular()
{


}

void WorldGenController::create_chunk_cache()
{
	double start = Utilities::Get_Time();

	for (int i = 0; i < m_max_cached_chunks; ++i) {
		ChunkRef chk = create_chunk_object();
		m_cached_chunks.push(chk);
	}

	double end = Utilities::Get_Time();;
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

	glm::fvec3 chunk_world_pos = chunkCoordToWorldPos(chunk_coord);

	chunk.chunk_obj->Name("Voxel Chunk (" + std::to_string(chunk_coord.x) + ", " + std::to_string(chunk_coord.y) + ", " + std::to_string(chunk_coord.z) + ")");
	chunk.chunk_obj->Get_Transform()->Translate(chunk_world_pos);

	m_create_queue.push(chunk);

	return true;
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

	std::ranges::sort(col_list, std::ranges::less{}, &glm::ivec3::z);

	return col_list;

}

glm::ivec3 WorldGenController::worldPosToChunkCoord(glm::fvec3 pos)
{
	return glm::ivec3(
		std::round(pos.x / m_chunkMeterSizeX + m_half), 
		std::round(pos.y / m_chunkMeterSizeY + m_half),
		std::round(pos.z / m_chunkMeterSizeZ + m_half)
	);
}

glm::fvec3 WorldGenController::chunkCoordToWorldPos(glm::ivec3 chunk_coord)
{
	return glm::fvec3(
		std::round(chunk_coord.x * m_chunkMeterSizeX - m_half),
		std::round(chunk_coord.y * m_chunkMeterSizeY - m_half),
		std::round(chunk_coord.z * m_chunkMeterSizeZ - m_half)
	);
}


