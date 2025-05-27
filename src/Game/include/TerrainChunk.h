#pragma once

#include "game_engine.h"


class Opaque_Chunk_Material;
class Stitch_VBO;
class WorldGenController;

class TerrainChunk : public Component
{
public:

	void Init(WorldGenController* controller, Stitch_VBO* vbo_stitch);

	void Assign(glm::ivec3 chunk_coord);

	void Unassign();

	void Process_Mesh_Update(glm::ivec4 counts);

protected:
	void Init() override;

	void Update(float dt) override;

private:

	glm::ivec3 m_chunk_coords;
	glm::fvec3 m_chunk_world_pos;
	glm::ivec4 m_counts;
	WorldGenController* m_controller;
	Stitch_VBO* m_vbo_stitch;
	bool m_assigned{ false };

	WorldObject* m_opaque_chunk_obj{nullptr};
	Mesh* m_voxel_opaque_mesh{ nullptr };

	void test_despawn();

	void draw_debug_cube();

};