#pragma once

#include "game_engine.h"

#include "shared_structures.h"


#include <unordered_map>
#include <vector>
#include <queue>

#define DEFAULT_METER_SIZE 32.0f
#define DEFAULT_VOXELS_PER_METER 1.0f
#define MAX_CACHED_CHUNKS 100
#define DEFAULT_PROCESS_TIME_INIT_CREATE_MS 250.0
#define DEFAULT_PROCESS_TIME_RUNTIME_CREATE_MS 16.0


class StructureChunk;
class StructureDataStorage;
class Opaque_Structure_Chunk_Material;

namespace VoxelEngine {
	class CubeVoxelBuilder;
}

using namespace VoxelEngine;

class StructureController : public Component {

public:

	struct StructureMod {
	public:
		uint32_t Type;
		uint16_t Orientation;
		bool Change_Type;
		glm::ivec3 Voxel;

		glm::ivec3 chunk_coord;

		StructureMod(glm::ivec3 voxel) {
			Voxel = voxel;
			Orientation = 0;
			Type = 0;
			Change_Type = false;
		}

		StructureMod(glm::ivec3 voxel, uint32_t type);
	};

	static StructureController* Instance() { return m_Instance; }

	void Spawn_Chunk(glm::ivec3 chunk_coord) { queue_chunk_create(chunk_coord); }

	void Despawn_Chunk(glm::ivec3 chunk_coord) { queue_chunk_delete(chunk_coord); }

	bool Chunk_Exists(glm::ivec3 chunk_coord) { return chunk_exists(chunk_coord); }

	Opaque_Structure_Chunk_Material* Get_Opaque_Chunk_Material() { return m_chunk_opaque_mat; }
	
	static glm::ivec3 WorldPosToChunkCoord(glm::fvec3 pos) { return m_Instance->worldPosToChunkCoord(pos); }

	static glm::fvec3 ChunkCoordToWorldPos(glm::ivec3 chunk_coord) { return m_Instance->chunkCoordToWorldPos(chunk_coord); }

	static glm::ivec3 VoxelToChunk(glm::ivec3 location) { return m_Instance->voxelToChunk(location); }

	static glm::ivec3 ChunkToVoxel(glm::ivec3 location) { return m_Instance->chunkToVoxel(location); }

	static glm::ivec3 GlobalToLocalChunkCoord(glm::ivec3 location) { return m_Instance->globalToLocalChunkCoord(location); }

	static glm::ivec3 GlobalToLocalChunkCoord(glm::ivec3 ChunkCoord, glm::ivec3 location) { return m_Instance->globalToLocalChunkCoord(ChunkCoord, location); }

	static glm::ivec3 LocalToGlobalCoord(glm::ivec3 Chunk, glm::ivec3 location) { return m_Instance->localToGlobalCoord(Chunk, location); }

	static glm::fvec3 VoxelToWorld(glm::ivec3 loc) { return m_Instance->voxelToWorld(loc); }

	static glm::ivec3 WorldToVoxel(glm::fvec3 worldPos) { return m_Instance->worldToVoxel(worldPos); }

	static int Hash_Chunk(glm::ivec3 chunk);

protected:
	void Init() override;

	void Update(float dt) override;

private:

	struct ChunkRef {
		glm::ivec3 chunk_coord;
		WorldObject* chunk_obj;
		StructureChunk* chunk_comp;
	};

	struct TerrainModEntry {
	public:
		std::vector<StructureMod> changes;
		glm::ivec3 chunk;
	};

	double m_voxelsPerMeter{ DEFAULT_VOXELS_PER_METER };
	double m_chunkMeterSizeX{ DEFAULT_METER_SIZE };
	double m_chunkMeterSizeY{ DEFAULT_METER_SIZE };
	double m_chunkMeterSizeZ{ DEFAULT_METER_SIZE };
	double m_half;

	int m_chunk_size_x{ 0 };
	int m_chunk_size_y{ 0 };
	int m_chunk_size_z{ 0 };

	int m_max_cached_chunks{ MAX_CACHED_CHUNKS };

	bool m_async_run{ false };
	std::thread m_process_thread;

	static StructureController* m_Instance;

	Opaque_Structure_Chunk_Material* m_chunk_opaque_mat{ nullptr };

	ChunkSettings settings;
	CubeVoxelBuilder* m_builder{ nullptr };
	StructureDataStorage* m_data_storage{ nullptr };

	std::queue<ChunkRef> m_cached_chunks;
	std::unordered_map<int, ChunkRef> m_chunk_map;

	std::queue<ChunkRef> m_create_queue;
	std::queue<glm::ivec3> m_delete_queue;

	std::queue<TerrainModEntry> m_terrain_change_queue;

	double m_process_time_ms{ DEFAULT_PROCESS_TIME_RUNTIME_CREATE_MS };

	void start();

	static void Run(StructureController* inst);

	void run_loop();

	void async_update();

	void initialize_voxel_engine();

	void process_additions();

	void process_deletions();

	void process_chunk(ChunkRef chunk);

	bool queue_chunk_create(glm::ivec3 chunk_coord);

	void queue_chunk_delete(glm::ivec3 chunk_coord);

	bool chunk_exists(glm::ivec3 chunk_coord);

	void remove_chunk(glm::ivec3 chunk_coord);

	void create_chunk_cache();

	ChunkRef create_chunk_object();


	glm::ivec3 worldPosToChunkCoord(glm::fvec3 pos);

	glm::fvec3 chunkCoordToWorldPos(glm::ivec3 chunk_coord);

	glm::ivec3 voxelToChunk(glm::ivec3 location);

	glm::ivec3 chunkToVoxel(glm::ivec3 location);

	glm::ivec3 globalToLocalChunkCoord(glm::ivec3 location);

	glm::ivec3 globalToLocalChunkCoord(glm::ivec3 ChunkCoord, glm::ivec3 location);

	glm::ivec3 localToGlobalCoord(glm::ivec3 Chunk, glm::ivec3 location);

	glm::fvec3 voxelToWorld(glm::ivec3 loc);

	glm::ivec3 worldToVoxel(glm::fvec3 worldPos);

	inline static const std::string LOG_LOC{ "STRUCTURE_CONTROLLER" };

};
