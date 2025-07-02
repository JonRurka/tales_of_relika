#include "StructureChunk.h"

#include "CubeVoxelBuilder.h"
#include "StructureController.h"
#include "Opaque_Structure_Chunk_Material.h"
#include "Mesh.h"

#define VBO_ELEMENTS 3
#define STRIDE (VBO_ELEMENTS * sizeof(float) * 4)

using namespace VoxelEngine;

void StructureChunk::Init(StructureController* controller)
{
	m_controller = controller;

	int max_vert = (int)Utilities::Vertex_Limit_Mode::Chunk_Max;
	m_voxel_opaque_mesh = new Mesh(max_vert * STRIDE);

	m_opaque_chunk_obj = Instantiate("Cached Voxel Chunk - Opaque");
	m_opaque_chunk_obj->Get_Transform()->Set_Verbos(false);
	//obj->Get_MeshRenderer()->Transparent(true);
	m_opaque_chunk_obj->Get_Transform()->Position(glm::vec3(0.0, 1000.0, 0.0));
	m_opaque_chunk_obj->Get_MeshRenderer()->Set_Material(controller->Get_Opaque_Chunk_Material());
	m_opaque_chunk_obj->Get_MeshRenderer()->Set_Mesh(m_voxel_opaque_mesh, false);
	m_opaque_chunk_obj->Add_Component<MeshCollider>();
}

void StructureChunk::Assign(glm::ivec3 chunk_coord)
{
	//m_vertex = new glm::vec4[(int)Utilities::Vertex_Limit_Mode::Chunk_Max];
	//m_normal = new glm::vec4[(int)Utilities::Vertex_Limit_Mode::Chunk_Max];
	//m_uv = new glm::vec2[(int)Utilities::Vertex_Limit_Mode::Chunk_Max];
	//m_trianges = new unsigned int[(int)(Utilities::Vertex_Limit_Mode::Chunk_Max) * 3];

	//m_vertex = std::vector<glm::vec4>((int)Utilities::Vertex_Limit_Mode::Chunk_Max, glm::vec4());
	//m_normal = std::vector<glm::vec4>((int)Utilities::Vertex_Limit_Mode::Chunk_Max, glm::vec4());
	//m_uv = std::vector<glm::vec2>((int)Utilities::Vertex_Limit_Mode::Chunk_Max, glm::vec2());
	//m_trianges = std::vector<unsigned int>((int)Utilities::Vertex_Limit_Mode::Chunk_Max, 0);

	m_vertex = std::vector<glm::vec4>();
	m_normal = std::vector<glm::vec4>();
	m_uv = std::vector<glm::vec2>();
	m_trianges = std::vector<unsigned int>();

	m_vertex.reserve((int)Utilities::Vertex_Limit_Mode::Chunk_Max);
	m_normal.reserve((int)Utilities::Vertex_Limit_Mode::Chunk_Max);
	m_uv.reserve((int)Utilities::Vertex_Limit_Mode::Chunk_Max);
	m_trianges.reserve((int)Utilities::Vertex_Limit_Mode::Chunk_Max);

	m_assigned = true;

	m_chunk_coords = chunk_coord;
	Object()->Name("Structure Chunk (" + std::to_string(chunk_coord.x) + ", " + std::to_string(chunk_coord.y) + ", " + std::to_string(chunk_coord.z) + ")");
	m_opaque_chunk_obj->Name("Structure Chunk - Opaque (" + std::to_string(chunk_coord.x) + ", " + std::to_string(chunk_coord.y) + ", " + std::to_string(chunk_coord.z) + ")");

	m_chunk_world_pos = StructureController::ChunkCoordToWorldPos(chunk_coord);

	Object()->Get_Transform()->Position(m_chunk_world_pos);
	m_opaque_chunk_obj->Get_Transform()->Position(m_chunk_world_pos);
}

void StructureChunk::Unassign()
{
}

void StructureChunk::Process_Mesh_Update(CubeVoxelBuilder* builder)
{
	ChunkRenderOptions render_options;

	render_options.locations.push_back(m_chunk_coords);

	glm::dvec4 render_times = builder->Render(&render_options, m_vertex, m_normal, m_uv, m_trianges, m_counts);

	Chunk_Mesh_Data data{};
	data.vertices = m_vertex;
	data.normals = m_normal;
	data.indices = m_trianges;
	data.counts = m_counts;

	m_data_lock.lock();
	m_chunk_process_queue.push(data);
	m_data_lock.unlock();
}

bool StructureChunk::Collision_Enabled()
{
	return true;
}

void StructureChunk::Init()
{


}

void StructureChunk::Update(float dt)
{
	if (!m_assigned)
		return;

	apply_mesh_update();
	update_collision_mesh();
}

void StructureChunk::VoxelChanged(glm::ivec3 local_voxel, bool Type_changed, uint32_t type)
{
}

void StructureChunk::apply_mesh_update()
{

	bool has_update = false;
	Chunk_Mesh_Data data{};

	m_data_lock.lock();
	while (!m_chunk_process_queue.empty()) {
		// Only take latest update.
		data = m_chunk_process_queue.front();
		m_chunk_process_queue.pop();
		has_update = true;
	}
	m_data_lock.unlock();

	if (!has_update) {
		return;
	}

	// TODO: case where there was a previous mesh.
	if (data.counts.x <= 0) {
		return;
	}

	Logger::LogDebug(LOG_POS("apply_mesh_update"), "verts:(%i), norms:(%i)", data.vertices.size(), data.normals.size());
	
	m_voxel_opaque_mesh->Vertices(data.vertices, false);
	m_voxel_opaque_mesh->Normals(data.normals, false);
	//m_voxel_opaque_mesh->TexCoords(data.);
	//m_voxel_opaque_mesh->Indices(data.indices);
	m_voxel_opaque_mesh->Activate(true);

	m_update_collision_mesh = true;

	draw_debug_cube();
}

void StructureChunk::update_collision_mesh()
{
	if (!Collision_Enabled()) {
		if (m_has_collision) {
			// TODO: remove collision
			m_has_collision = true;
		}
		return;
	}

	if (!m_update_collision_mesh) {
		return;
	}
	m_update_collision_mesh = false;

	if (m_mesh_collider == nullptr) {
		m_mesh_collider = m_opaque_chunk_obj->Add_Component<MeshCollider>();
	}

	btVector3 min, max;

	m_collision_mesh = new Mesh();
	//m_collision_mesh->Indices(tris);
	m_collision_mesh->Vertices(m_voxel_opaque_mesh->Vertices());
	m_collision_mesh->Activate();
	m_mesh_collider->SetMesh(m_collision_mesh);
	m_mesh_collider->Mass(0.0f);
	m_mesh_collider->Activate();
	m_mesh_collider->RigidBody()->getAabb(min, max);
	Logger::LogDebug(LOG_POS("update_collision_mesh"), "Collision mesh added.");

}

Mesh::VertexAttributeList StructureChunk::get_vertex_attributes()
{
	Mesh::VertexAttributeList res(STRIDE);
	res.add_attribute(4, 0);
	res.add_attribute(4, (4 * sizeof(float)));
	res.add_attribute(4, (8 * sizeof(float)));
	return res;
}

void StructureChunk::draw_debug_cube()
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

