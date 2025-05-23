#pragma once

#include "game_engine.h"

#include <unordered_map>
#include <vector>
#include <queue>

class Opaque_Chunk_Material;
class TerrainChunk;

#define DEFAULT_METER_SIZE 32.0f
#define DEFAULT_VOXELS_PER_METER 1.0f
#define DEFAULT_MAX_CHUNK_RADIUS 10
#define DEFAULT_DEPTH 5

class WorldGenController : public Component
{
public:

	void SetTarget(Transform* target) { mTarget = target; }

	void Start();

protected:
	void Init() override;

	void Update(float dt) override;

private:

	struct ChunkRef {
		WorldObject* chunk_obj;
		TerrainChunk* chunk_comp;
	};

	Transform* mTarget{nullptr};

	double m_voxelsPerMeter{ DEFAULT_VOXELS_PER_METER };
	double m_chunkMeterSizeX{ DEFAULT_METER_SIZE };
	double m_chunkMeterSizeY{ DEFAULT_METER_SIZE };
	double m_chunkMeterSizeZ{ DEFAULT_METER_SIZE };
	double m_half;

	int m_chunks_depth{ DEFAULT_DEPTH };

	int m_max_chunk_radius{ DEFAULT_MAX_CHUNK_RADIUS };
	int m_max_cached_columns{ 0 };
	int m_max_column_target{ 0 };

	int m_max_cached_chunks{ 0 };

	std::queue<ChunkRef> m_cached_chunks;
	std::unordered_map<int, ChunkRef> m_chunk_map;

	std::queue<ChunkRef> m_create_queue;

	Opaque_Chunk_Material* m_chunk_opaque_mat{ nullptr };

	ChunkRef create_chunk_object();

	void generate_circular();

	void cull_circular();

	void create_chunk_cache();

	bool queue_chunk_create(glm::ivec3 chunk_coord);

	std::vector<glm::ivec3> get_columns_in_radius(int center_x, int center_z, int radius);

	glm::ivec3 worldPosToChunkCoord(glm::fvec3 pos);

	glm::fvec3 chunkCoordToWorldPos(glm::ivec3 chunk_coord);


	inline static const std::string LOG_LOC{ "WORLD_GEN_CONTROLLER" };
};