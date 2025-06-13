#include "TerrainChunk.h"

#include "Stitch_VBO.h"
#include "WorldGenController.h"
#include "Opaque_Chunk_Material.h"

#define DRAW_DEBUG_BOX true

void TerrainChunk::Init()
{
	

}

void TerrainChunk::Init(WorldGenController* controller, Stitch_VBO* vbo_stitch)
{
	m_controller = controller;
	m_vbo_stitch = vbo_stitch;

	int max_vert = (int)Utilities::Vertex_Limit_Mode::Chunk_Max;
	m_voxel_opaque_mesh = new Mesh(max_vert * Stitch_VBO::Stride());

	m_opaque_chunk_obj = Instantiate("Cached Voxel Chunk - Opaque");
	m_opaque_chunk_obj->Get_Transform()->Set_Verbos(false);
	//obj->Get_MeshRenderer()->Transparent(true);
	m_opaque_chunk_obj->Get_Transform()->Position(glm::vec3(0.0, 1000.0, 0.0));
	m_opaque_chunk_obj->Get_MeshRenderer()->Set_Material(controller->Get_Chunk_Material());
	m_opaque_chunk_obj->Get_MeshRenderer()->Set_Mesh(m_voxel_opaque_mesh);
	m_opaque_chunk_obj->Add_Component<MeshCollider>();
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
	m_vbo_stitch->Process(m_voxel_opaque_mesh, counts, false);
}

void TerrainChunk::Update(float dt)
{
	test_despawn();
}

void TerrainChunk::test_despawn()
{
	glm::vec3 target_pos = m_controller->Target_Position();
	glm::ivec3 target_chunk = WorldGenController::WorldPosToChunkCoord(target_pos);

	glm::fvec2 target_chunk_f = glm::fvec2(target_chunk.x, target_chunk.z);
	glm::fvec2 chunk_coord_f = glm::fvec2(m_chunk_coords.x, m_chunk_coords.z);

	float dist = glm::distance(target_chunk_f, chunk_coord_f);
	if (dist > m_controller->Chunk_Radius()) 
	{
		m_controller->Despawn_Chunk(m_chunk_coords);
	}
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

