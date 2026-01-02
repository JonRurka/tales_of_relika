#pragma once

#include "game_engine.h"

#include "shared_structures.h"

#include "StructureDataStorage.h"
#include "Opaque_Structure_Chunk_Material.h"
#include "CubeVoxelBuilder.h"

#include <unordered_map>
#include <vector>
#include <queue>
#include <thread>
#include <memory>

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
	typedef std::shared_ptr<StructureController> Shared;
	typedef std::weak_ptr<StructureController> Weak;

	struct StructureMod {
	public:
		uint32_t Type;
		bool Change_Type;
		glm::ivec3 Voxel;

		glm::ivec3 chunk_coord;

		StructureMod(glm::ivec3 voxel) {
			Voxel = voxel;
			Type = 0;
			Change_Type = false;
			chunk_coord = glm::ivec3();
		}

		StructureMod(glm::ivec3 voxel, uint32_t type);
	};

	static StructureController* Instance() { return m_Instance; }

	void Spawn_Chunk(glm::ivec3 chunk_coord) { queue_chunk_create(chunk_coord); }

	void Spawn_Chunk(glm::ivec3 chunk_coord, std::vector<StructureMod> values) { queue_chunk_create(chunk_coord, values); }

	void Spawn_Chunk(glm::ivec3 chunk_coord, std::vector<uint32_t> data) { queue_chunk_create(chunk_coord, data); }

	void Despawn_Chunk(glm::ivec3 chunk_coord) { queue_chunk_delete(chunk_coord); }

	bool Chunk_Exists(glm::ivec3 chunk_coord) { return chunk_exists(chunk_coord); }

	Opaque_Structure_Chunk_Material::Shared Get_Opaque_Chunk_Material() { return m_chunk_opaque_mat; }

	void Refresh_Chunk(glm::ivec3 chunk);
	void Modify_Voxel_Type(glm::ivec3 voxel, uint32_t type);
	void Modify_Voxel(std::vector<StructureMod> values);
	void Modify_Voxel(glm::ivec3 chunk, StructureMod value, bool update_neighbor = true);
	void Modify_Voxel(glm::ivec3 chunk, std::vector<StructureMod> values, bool update_neighbor = true);

	void Submit_Structure_Modification(glm::ivec3 chunk, StructureMod value);
	void Submit_Structure_Modification(glm::ivec3 chunk, std::vector<StructureMod> values);
	
	StructureDataStorage::Shared Get_Data_Storage() { return m_data_storage; }

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

	void OnDestroy() override;

private:

	struct ChunkRef {
		glm::ivec3 chunk_coord;
		WorldObject::Weak chunk_obj;
		std::weak_ptr<StructureChunk> chunk_comp;
	};

	struct ChunkCreationRequest {
		ChunkRef Chunk;
		std::vector<uint32_t> Initial_Data;
	};

	struct StructureModEntry {
	public:
		std::vector<StructureMod> changes;
		glm::ivec3 chunk;
	};

	double m_voxelsPerMeter{ DEFAULT_VOXELS_PER_METER };
	double m_chunkMeterSizeX{ DEFAULT_METER_SIZE };
	double m_chunkMeterSizeY{ DEFAULT_METER_SIZE };
	double m_chunkMeterSizeZ{ DEFAULT_METER_SIZE };
	double m_half{ 0 };

	int m_chunk_size_x{ 0 };
	int m_chunk_size_y{ 0 };
	int m_chunk_size_z{ 0 };

	int m_max_cached_chunks{ MAX_CACHED_CHUNKS };

	bool m_async_run{ false };
	std::thread m_process_thread;

	static StructureController* m_Instance;

	Opaque_Structure_Chunk_Material::Shared m_chunk_opaque_mat;

	ChunkSettings settings;
	CubeVoxelBuilder::Shared m_builder;
	StructureDataStorage::Shared m_data_storage;

	std::queue<ChunkRef> m_cached_chunks;
	std::unordered_map<int, ChunkRef> m_chunk_map;

	std::queue<ChunkCreationRequest> m_create_queue;
	std::queue<glm::ivec3> m_delete_queue;

	std::queue<StructureModEntry> m_terrain_change_queue;

	double m_process_time_ms{ DEFAULT_PROCESS_TIME_RUNTIME_CREATE_MS };

	void start();

	static void Run();

	void run_loop();

	void async_update();

	void initialize_voxel_engine();

	void process_additions();

	void process_deletions();

	void process_modifications();

	void process_chunk(ChunkRef chunk);

	bool queue_chunk_create(glm::ivec3 chunk_coord);

	bool queue_chunk_create(glm::ivec3 chunk_coord, std::vector<StructureMod> values);

	bool queue_chunk_create(glm::ivec3 chunk_coord, std::vector<uint32_t> data);

	void queue_chunk_delete(glm::ivec3 chunk_coord);

	bool chunk_exists(glm::ivec3 chunk_coord);

	void remove_chunk(glm::ivec3 chunk_coord);

	void create_chunk_cache();

	ChunkRef create_chunk_object();

	ChunkRef get_chunk(glm::ivec3 coord);

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
