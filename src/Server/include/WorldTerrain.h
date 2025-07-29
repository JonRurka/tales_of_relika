#pragma once

#include "dynamic_compute.h"

#include "shared_structures.h"
#include "IVoxelBuilder.h"
#include "SmoothVoxelBuilder.h"

#include <unordered_map>
#include <vector>
#include <queue>

using namespace VoxelEngine;
using namespace DynamicCompute::Compute;

#define DEFAULT_METER_SIZE 32.0f
#define DEFAULT_VOXELS_PER_METER 1.0f
#define DEFAULT_MAX_CHUNK_RADIUS 10
#define DEFAULT_INITIAL_CHUNK_CACHE 1000
#define DEFAULT_BATCH_SIZE 1
#define MAX_BATCH_SIZE 1
#define DEFAULT_PROCESS_TIME_RUNTIME_CREATE_MS 16.0

class ServerTerrainChunk;
class World;

class WorldTerrain {
public:

	struct TerrainMod {
	public:
		float ISO;
		int Type;
		bool Change_ISO;
		bool Change_Type;
		glm::ivec3 Voxel;

		glm::ivec3 chunk_coord;

		TerrainMod(glm::ivec3 voxel) {
			Voxel = voxel;
			ISO = 0;
			Type = 0;
			Change_ISO = false;
			Change_Type = false;
		}

		TerrainMod(glm::ivec3 voxel, float iso, int type) {
			Voxel = voxel;
			ISO = iso;
			Type = type;
			Change_ISO = true;
			Change_Type = true;
		}

		TerrainMod(glm::ivec3 voxel, float iso) {
			Voxel = voxel;
			ISO = iso;
			Type = 0;
			Change_ISO = true;
			Change_Type = false;
		}

		TerrainMod(glm::ivec3 voxel, int type) {
			Voxel = voxel;
			ISO = 0;
			Type = type;
			Change_ISO = false;
			Change_Type = true;
		}
	};

	glm::ivec3 WorldPosToChunkCoord(glm::fvec3 pos) { return worldPosToChunkCoord(pos); }

	glm::fvec3 ChunkCoordToWorldPos(glm::ivec3 chunk_coord) { return chunkCoordToWorldPos(chunk_coord); }

	glm::ivec3 VoxelToChunk(glm::ivec3 location) { return voxelToChunk(location); }

	glm::ivec3 ChunkToVoxel(glm::ivec3 location) { return chunkToVoxel(location); }

	glm::ivec3 GlobalToLocalChunkCoord(glm::ivec3 location) { return globalToLocalChunkCoord(location); }

	glm::ivec3 GlobalToLocalChunkCoord(glm::ivec3 ChunkCoord, glm::ivec3 location) { return globalToLocalChunkCoord(ChunkCoord, location); }

	glm::ivec3 LocalToGlobalCoord(glm::ivec3 Chunk, glm::ivec3 location) { return localToGlobalCoord(Chunk, location); }

	glm::fvec3 VoxelToWorld(glm::ivec3 loc) { return voxelToWorld(loc); }

	glm::ivec3 WorldToVoxel(glm::fvec3 worldPos) { return worldToVoxel(worldPos); }

	static int Hash_Chunk(glm::ivec3 chunk);

	WorldTerrain();

	ServerTerrainChunk* Spawn_Chunk(glm::ivec3 chunk_coord) { return queue_chunk_create(chunk_coord); }

	void Despawn_Chunk(glm::ivec3 chunk_coord) { queue_chunk_delete(chunk_coord); }

	bool Chunk_Exists(glm::ivec3 chunk_coord) { return chunk_exists(chunk_coord); }

	void Refresh_Chunk(glm::ivec3 chunk);
	void Modify_Voxel_ISO(glm::ivec3 voxel, float iso);
	void Modify_Voxel_Type(glm::ivec3 voxel, int type);
	void Modify_Voxel(std::vector<TerrainMod> values);
	void Modify_Voxel(glm::ivec3 chunk, TerrainMod value, bool update_neighbor = true);
	void Modify_Voxel(glm::ivec3 chunk, std::vector<TerrainMod> values, bool update_neighbor = true);

	void Submit_Terrain_Modification(glm::ivec3 chunk, TerrainMod value);
	void Submit_Terrain_Modification(glm::ivec3 chunk, std::vector<TerrainMod> values);

	ServerTerrainChunk* Get_Chunk(glm::ivec3 chunk_coord);

	std::vector<glm::ivec3> Get_Chunk_Coords(glm::fvec3 loc, int radius, int depth);

	World* Get_World() { return m_world; }

	TerrainModifications* Get_TerrainModifications() { return m_terrain_mods; }

	HeightmapGenerator* Get_HeightmapGenerator() { return m_heightmap_gen; }

	void Init(World* world);

	void Update(float dt);

private:

	struct ChunkRef {
		glm::ivec3 chunk_coord;
		ServerTerrainChunk* chunk_comp;
	};

	struct TerrainModEntry {
	public:
		std::vector<TerrainMod> changes;
		glm::ivec3 chunk;
	};

	IVoxelBuilder_private* m_builder{ nullptr };
	TerrainModifications* m_terrain_mods{ nullptr };
	HeightmapGenerator* m_heightmap_gen{ nullptr };
	ChunkSettings settings;

	World* m_world{nullptr};

	double m_voxelsPerMeter{ DEFAULT_VOXELS_PER_METER };
	double m_chunkMeterSizeX{ DEFAULT_METER_SIZE };
	double m_chunkMeterSizeY{ DEFAULT_METER_SIZE };
	double m_chunkMeterSizeZ{ DEFAULT_METER_SIZE };
	double m_half;

	int m_chunk_size_x{ 0 };
	int m_chunk_size_y{ 0 };
	int m_chunk_size_z{ 0 };

	int m_batch_size{ DEFAULT_BATCH_SIZE };

	int m_initial_cached_columns{ DEFAULT_INITIAL_CHUNK_CACHE };

	double m_process_time_ms{ DEFAULT_PROCESS_TIME_RUNTIME_CREATE_MS };

	glm::vec4* m_vertices{ nullptr };
	glm::vec4* m_normals{ nullptr };
	glm::vec4* m_mats{ nullptr };
	unsigned int* m_triangles{ nullptr };

	std::queue<ChunkRef> m_cached_chunks;
	std::unordered_map<int, ChunkRef> m_chunk_map;

	std::queue<ChunkRef> m_create_queue;
	std::queue<glm::ivec3> m_delete_queue;

	std::queue<TerrainModEntry> m_terrain_change_queue;

	ChunkRef get_chunk(glm::ivec3 coord);

	void initialize_voxel_engine();

	void process_additions();

	void process_deletions();

	bool process_batch();

	void process_modifications();

	void create_chunk_cache();

	ChunkRef create_chunk_object();

	ServerTerrainChunk* queue_chunk_create(glm::ivec3 chunk_coord);

	void queue_chunk_delete(glm::ivec3 chunk_coord);

	bool chunk_exists(glm::ivec3 chunk_coord);

	void remove_chunk(glm::ivec3 chunk_coord);

	void compute_triangles();

	std::vector<glm::ivec3> get_columns_in_radius(int center_x, int center_z, int radius);

	glm::ivec3 worldPosToChunkCoord(glm::fvec3 pos);

	glm::fvec3 chunkCoordToWorldPos(glm::ivec3 chunk_coord);

	glm::ivec3 voxelToChunk(glm::ivec3 location);

	glm::ivec3 chunkToVoxel(glm::ivec3 location);

	glm::ivec3 globalToLocalChunkCoord(glm::ivec3 location);

	glm::ivec3 globalToLocalChunkCoord(glm::ivec3 ChunkCoord, glm::ivec3 location);

	glm::ivec3 localToGlobalCoord(glm::ivec3 Chunk, glm::ivec3 location);

	glm::fvec3 voxelToWorld(glm::ivec3 loc);

	glm::ivec3 worldToVoxel(glm::fvec3 worldPos);

	inline static const std::string LOG_LOC{ "SERVER_WORLD_TERRAIN" };
};