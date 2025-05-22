#pragma once

#include "game_engine.h"

#include <unordered_map>
#include <vector>

class Opaque_Chunk_Material;
class TerrainChunk;

class WorldGenController : public Component
{
public:

protected:
	void Init() override;

	void Update(float dt) override;

private:

	struct ChunkRef {
		WorldObject* chunk_obj;
		TerrainChunk* chunk_comp;
	};

	std::unordered_map<int, ChunkRef> m_chunk_map;

	Opaque_Chunk_Material* m_chunk_opaque_mat{ nullptr };

	WorldObject* create_chunk_object(int x, int y, int z);

	std::vector<glm::ivec3> get_columns_in_radius(int center_x, int center_z, int radius);

	inline static const std::string LOG_LOC{ "WORLD_GEN_CONTROLLER" };
};