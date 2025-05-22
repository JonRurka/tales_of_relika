#include "WorldGenController.h"

#include "Stitch_VBO.h"
#include "Opaque_Chunk_Material.h"
#include "TerrainChunk.h"

#include <algorithm>

void WorldGenController::Init()
{
	Logger::LogInfo(LOG_POS("Init"), "Initialized");

	m_chunk_opaque_mat = new Opaque_Chunk_Material();
	m_chunk_opaque_mat->SetVec3("material.ambientColor", glm::vec3(1.0f, 0.5f, 0.31f));
	m_chunk_opaque_mat->SetVec3("material.diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_chunk_opaque_mat->SetVec2("material.scale", glm::vec2(1.0f, 1.0f));
	m_chunk_opaque_mat->setFloat("material.shininess", 32.0f);
	m_chunk_opaque_mat->setFloat("material.specular_intensity", 1.0f);
	m_chunk_opaque_mat->SetVec3("globalAmbientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_chunk_opaque_mat->setFloat("globalAmbientIntensity", 0.1f);

	double start = Utilities::Get_Time();
	std::vector<glm::ivec3> cols = get_columns_in_radius(0, 0, 100);
	double end = Utilities::Get_Time();
	Logger::LogDebug(LOG_POS("Init"), "Got %i columns in %f ms", cols.size(), (end - start) * 1000);

	for (int i = 0; i < cols.size(); i++) {
		//Logger::LogDebug(LOG_POS("Init"), "coord: (%i, %i, %i)",
		//	cols[i].x, cols[i].y, cols[i].z);
	}
}

void WorldGenController::Update(float dt)
{
	


}

WorldObject* WorldGenController::create_chunk_object(int x, int y, int z)
{
	int hash = Utilities::Hash_Chunk_Coord(x, y, z);

	if (m_chunk_map.contains(hash)) {
		return m_chunk_map[hash].chunk_obj;
	}

	int max_vert = (int)Utilities::Vertex_Limit_Mode::Chunk_Max;
	Mesh* voxel_mesh_test = new Mesh(max_vert * Stitch_VBO::Stride());

	WorldObject* obj = Instantiate("Voxel Chunk (" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")");
	obj->Get_Transform()->Set_Verbos(true);
	obj->Get_MeshRenderer()->Set_Mesh(voxel_mesh_test);
	//obj->Get_MeshRenderer()->Transparent(true);
	//obj->Get_MeshRenderer()->Set_Shader(m_shader);
	obj->Get_Transform()->Translate(x * 32, y * 32, z * 32);
	obj->Get_MeshRenderer()->Set_Material(m_chunk_opaque_mat);
	//obj->Add_Component<MeshCollider>();
	TerrainChunk* comp = obj->Add_Component<TerrainChunk>();

	ChunkRef chk{};
	chk.chunk_obj = obj;
	chk.chunk_comp = comp;
	m_chunk_map[hash] = chk;

	return obj;
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
