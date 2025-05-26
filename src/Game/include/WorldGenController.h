#pragma once

#include "game_engine.h"

#include "dynamic_compute.h"

#include "shared_structures.h"
#include "IVoxelBuilder.h"
#include "SmoothVoxelBuilder.h"

#include <unordered_map>
#include <vector>
#include <queue>

using namespace VoxelEngine;
using namespace DynamicCompute::Compute;

class Opaque_Chunk_Material;
class TerrainChunk;
class Stitch_VBO;

#define DEFAULT_METER_SIZE 32.0f
#define DEFAULT_VOXELS_PER_METER 1.0f
#define DEFAULT_MAX_CHUNK_RADIUS 10
#define DEFAULT_DEPTH 6
#define DEFAULT_BATCH_SIZE 4
#define MAX_BATCH_SIZE 4
#define DEFAULT_PROCESS_TIME_INIT_CREATE_MS 100.0
#define DEFAULT_PROCESS_TIME_RUNTIME_CREATE_MS 16.0

class WorldGenController : public Component
{
public:

	void SetTarget(Transform* target) { mTarget = target; }

	void Start();

	static WorldGenController* Instance() { return m_Instance; }

	void Spawn_Chunk(glm::ivec3 chunk_coord) { queue_chunk_create(chunk_coord); }

	void Despawn_Chunk(glm::ivec3 chunk_coord) { queue_chunk_delete(chunk_coord); }

	bool Chunk_Exists(glm::ivec3 chunk_coord) { return chunk_exists(chunk_coord); };

	TerrainChunk* Get_Chunk(glm::ivec3 chunk_coord);

	Opaque_Chunk_Material* Get_Chunk_Material() { return m_chunk_opaque_mat; }

	int Chunk_Radius() { return m_max_chunk_radius; }

	glm::fvec3 Target_Position();

	static glm::ivec3 WorldPosToChunkCoord(glm::fvec3 pos) { return m_Instance->worldPosToChunkCoord(pos); }

	static glm::fvec3 ChunkCoordToWorldPos(glm::ivec3 chunk_coord) { return m_Instance->chunkCoordToWorldPos(chunk_coord); }

protected:
	void Init() override;

	void Update(float dt) override;

private:

	struct ChunkRef {
		glm::ivec3 chunk_coord;
		WorldObject* chunk_obj;
		TerrainChunk* chunk_comp;
	};

	Transform* mTarget{nullptr};

	static WorldGenController* m_Instance;

	IVoxelBuilder_private* m_builder{ nullptr };
	ChunkSettings settings;
	Stitch_VBO* vbo_stitch{ nullptr };

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

	int m_batch_size{ DEFAULT_BATCH_SIZE };
	double m_process_time_ms{ DEFAULT_PROCESS_TIME_INIT_CREATE_MS };

	double m_gen_start{ 0.0 };
	double m_gen_stop{ 0.0 };
	bool m_gen_finished{ false };

	bool m_world_gen_started{ false };

	std::queue<ChunkRef> m_cached_chunks;
	std::unordered_map<int, ChunkRef> m_chunk_map;

	std::queue<ChunkRef> m_create_queue;
	std::queue<glm::ivec3> m_delete_queue;

	Opaque_Chunk_Material* m_chunk_opaque_mat{ nullptr };


	ChunkRef get_chunk(glm::ivec3 coord);

	void initialize_voxel_engine();

	void process_additions();

	void process_deletions();

	bool process_batch();

	ChunkRef create_chunk_object();

	void generate_circular();

	void create_chunk_cache();

	bool queue_chunk_create(glm::ivec3 chunk_coord);

	void queue_chunk_delete(glm::ivec3 chunk_coord);

	bool chunk_exists(glm::ivec3 chunk_coord);

	void remove_chunk(glm::ivec3 chunk_coord);

	std::vector<glm::ivec3> get_columns_in_radius(int center_x, int center_z, int radius);

	glm::ivec3 worldPosToChunkCoord(glm::fvec3 pos);

	glm::fvec3 chunkCoordToWorldPos(glm::ivec3 chunk_coord);


	inline static const std::string LOG_LOC{ "WORLD_GEN_CONTROLLER" };
};