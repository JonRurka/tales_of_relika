#pragma once

#include "game_engine.h"

#include "dynamic_compute.h"

#include "shared_structures.h"
#include "IVoxelBuilder.h"
#include "SmoothVoxelBuilder.h"
#include "Stitch_VBO.h"
#include "Opaque_Chunk_Material.h"
#include "Standard_Material.h"

#include "Network/Data.h"
#include "Network/OpCodes.h"

#include <unordered_map>
#include <vector>
#include <queue>
#include <memory>
#include <algorithm>

using namespace VoxelEngine;
using namespace DynamicCompute::Compute;

class Opaque_Chunk_Material;
class TerrainChunk;
class Stitch_VBO;
class TerrainModifications;
class ISO_Sampler;
class GameClient;

#define DEFAULT_METER_SIZE 32.0f
#define DEFAULT_VOXELS_PER_METER 1.0f
#define DEFAULT_MAX_CHUNK_RADIUS 10
#define DEFAULT_DEPTH 4
#define DEFAULT_BATCH_SIZE 4
#define MAX_BATCH_SIZE 4
#define DEFAULT_PROCESS_TIME_INIT_CREATE_MS 250.0
#define DEFAULT_PROCESS_TIME_RUNTIME_CREATE_MS 8.0

#ifndef DECLARE_CHUNK_COMMAND
#define	DECLARE_CHUNK_COMMAND(type, func_name) 										 \
	static void func_name##_cb(void* obj, int hash, std::vector<uint8_t> data) {	 \
		type* cntrl = (type*)obj;													 \
		cntrl->func_name(hash, data);												 \
	}																				 \
	void func_name(int hash, std::vector<uint8_t> data); 
#endif

typedef void(*ChunkActionPtr)(void*, int, std::vector<uint8_t>);

class WorldGenController : public Component
{
public:
	typedef std::shared_ptr<WorldGenController> Shared;
	typedef std::weak_ptr<WorldGenController> Weak;

	enum MeshUpdateMode
	{
		Graphic = 1,
		Collision = 2,
		Both = 3,
	};

	struct TerrainMod {
	public:
		float ISO{ 0 };
		int Type{ 0 };
		bool Change_ISO{ false };
		bool Change_Type{ false };
		glm::ivec3 Voxel{ glm::ivec3() };
		MeshUpdateMode Mode{ MeshUpdateMode::Both };

		glm::ivec3 chunk_coord{ glm::ivec3() };

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

	struct Voxel_ISO {
		glm::ivec3 chunk_coord;
		glm::ivec3 local_voxel_coord;
		glm::ivec3 global_voxel_coord;
		glm::vec3 local_position;
		glm::vec3 world_position;
		float iso;

		static Voxel_ISO From_Voxel_Coord(const glm::ivec3& coord, float iso) {
			Voxel_ISO v_iso;
			v_iso.global_voxel_coord = coord;
			v_iso.chunk_coord = VoxelToChunk(coord);
			v_iso.local_voxel_coord = GlobalToLocalChunkCoord(v_iso.chunk_coord, v_iso.global_voxel_coord);
			v_iso.local_position = VoxelToWorld(v_iso.local_voxel_coord);
			v_iso.world_position = VoxelToWorld(v_iso.global_voxel_coord);
			v_iso.iso = iso;
			return v_iso;
		}
	};

	void InitCommands(std::weak_ptr<GameClient> client);

	void AddCommand(OpCodes::Player_Chunk_Events cmd, ChunkActionPtr callback, void* obj);

	void SetTarget(Transform::Weak target);

	void Start();

	static WorldGenController* Instance() { return m_Instance; }

	void Spawn_Chunk(glm::ivec3 chunk_coord) { queue_chunk_create(chunk_coord); }

	void Despawn_Chunk(glm::ivec3 chunk_coord) { queue_chunk_delete(chunk_coord); }

	bool Chunk_Exists(glm::ivec3 chunk_coord) { return chunk_exists(chunk_coord); };

	std::weak_ptr<TerrainChunk> Get_Chunk(glm::ivec3 chunk_coord);

	Opaque_Chunk_Material::Shared Get_Chunk_Material() { return m_chunk_opaque_mat; }

	ISO_Sampler::Shared Get_ISO_Sampler();

	bool Finished_Initial_Generation() { return m_gen_finished; }

	void Refresh_Chunk(glm::ivec3 chunk, MeshUpdateMode mode);
	void Modify_Voxel_ISO(glm::ivec3 voxel, float iso);
	void Modify_Voxel_Type(glm::ivec3 voxel, int type);
	void Modify_Voxel(std::vector<TerrainMod> values);
	void Modify_Voxel(glm::ivec3 chunk, TerrainMod value, bool update_neighbor = true);

	void Submit_Terrain_Modification(glm::ivec3 chunk, TerrainMod value);
	void Submit_Terrain_Modification(glm::ivec3 chunk, std::vector<TerrainMod> values);

	int Chunk_Radius() { return m_max_chunk_radius; }

	bool Terrain_Ready();

	glm::fvec3 Target_Position();

	glm::ivec3 Target_Chunk();

	glm::vec3 ChunkMeterSize() const
	{
		return glm::vec3(m_chunkMeterSizeX, m_chunkMeterSizeY, m_chunkMeterSizeZ);
	}

	glm::vec3 ChunkSize() const
	{
		return glm::vec3(m_chunk_size_x, m_chunk_size_y, m_chunk_size_z);
	}

	double VoxelsPerMeter() const
	{
		return m_voxelsPerMeter;
	}

	bool Voxel_Engine_Enabled() { return m_voxel_engine_enabled; }

	std::vector<Voxel_ISO> GetSurroundingVoxels(const glm::ivec3& src_voxel, int half_size);

	inline static glm::ivec3 WorldPosToChunkCoord(const glm::fvec3& pos) { return m_Instance->worldPosToChunkCoord(pos); }

	inline static glm::fvec3 ChunkCoordToWorldPos(const glm::ivec3& chunk_coord) { return m_Instance->chunkCoordToWorldPos(chunk_coord); }

	inline static glm::ivec3 VoxelToChunk(const glm::ivec3& location) { return m_Instance->voxelToChunk(location); }

	inline static glm::ivec3 ChunkToVoxel(const glm::ivec3& location) { return m_Instance->chunkToVoxel(location); }

	inline static glm::ivec3 GlobalToLocalChunkCoord(const glm::ivec3& location) { return m_Instance->globalToLocalChunkCoord(location); }

	inline static glm::ivec3 GlobalToLocalChunkCoord(const glm::ivec3& ChunkCoord, const glm::ivec3& location) { return m_Instance->globalToLocalChunkCoord(ChunkCoord, location); }

	inline static glm::ivec3 LocalToGlobalCoord(const glm::ivec3& Chunk, const glm::ivec3& location) { return m_Instance->localToGlobalCoord(Chunk, location); }

	inline static glm::fvec3 VoxelToWorld(const glm::ivec3& loc) { return m_Instance->voxelToWorld(loc); }

	inline static glm::ivec3 WorldToVoxel(const glm::fvec3& worldPos) { return m_Instance->worldToVoxel(worldPos); }

	inline static int Hash_Chunk(const glm::ivec3& chunk);


	bool K_Was_Hit() {
		return m_k_key_hit;
	}


	DECLARE_NET_CLIENT_COMMAND(WorldGenController, OnChunkEvent)

	/*static void OnChunkEvent_cb(void* obj, Data data) {
		WorldGenController* cntrl = (WorldGenController*)obj;
		cntrl->OnChunkEvent(data);
	}
	void OnChunkEvent(Data data);*/


	DECLARE_CHUNK_COMMAND(WorldGenController, OnChunkEvent_NotifyLoaded)
	DECLARE_CHUNK_COMMAND(WorldGenController, OnChunkEvent_NotifyUnloaded)

	/*static void OnChunkEvent_NotifyLoaded_cb(void* obj, int hash, std::vector<uint8_t> data) {
		WorldGenController* cntrl = (WorldGenController*)obj;
		cntrl->OnChunkEvent_NotifyLoaded(hash, data);
	}
	void OnChunkEvent_NotifyLoaded(int hash, std::vector<uint8_t> data);*/


protected:
	void Init() override;

	void Update(float dt) override;

	void OnDestroy() override;

private:

	struct ChunkCommand {
		void* Obj_Ptr;
		ChunkActionPtr Callback;
	};

	struct ChunkRef {
		glm::ivec3 chunk_coord;
		WorldObject::Weak chunk_obj;
		std::weak_ptr<TerrainChunk> chunk_comp;
	};

	struct TerrainModEntry {
	public:
		std::vector<TerrainMod> changes;
		glm::ivec3 chunk;		
	};

	Transform::Weak mTarget;

	static WorldGenController* m_Instance;

	std::unordered_map<uint8_t, ChunkCommand> m_commands;

	IVoxelBuilder_private::Shared m_builder;
	TerrainModifications::Shared m_terrain_mods;
	ChunkSettings settings;
	Stitch_VBO::Shared vbo_stitch;
	ISO_Sampler::Shared m_iso_sampler;

	double m_voxelsPerMeter{ DEFAULT_VOXELS_PER_METER };
	double m_chunkMeterSizeX{ DEFAULT_METER_SIZE };
	double m_chunkMeterSizeY{ DEFAULT_METER_SIZE };
	double m_chunkMeterSizeZ{ DEFAULT_METER_SIZE };
	double m_half{ 0 };

	int m_chunk_size_x{ 0 };
	int m_chunk_size_y{ 0 };
	int m_chunk_size_z{ 0 };

	int m_chunks_depth{ DEFAULT_DEPTH };

	int m_max_chunk_radius{ DEFAULT_MAX_CHUNK_RADIUS };
	int m_max_cached_columns{ 0 };
	int m_max_column_target{ 0 };

	int m_max_cached_chunks{ 0 };

	int m_batch_size{ DEFAULT_BATCH_SIZE };
	double m_process_time_ms{ DEFAULT_PROCESS_TIME_INIT_CREATE_MS };

	double m_gen_start{ 0.0 };
	double m_gen_stop{ 0.0 };
	volatile bool m_gen_finished{ false };
	bool m_initialized{ false };
	bool m_world_gen_started{ false };


	bool m_async{ false };
	volatile bool m_running{ false };
	std::thread m_process_thread;
	std::mutex m_process_lock;
	//int m_num_filled_chunks{ 0 };

	double m_chunk_init_all_gen_time_ms{ 0 };
	double m_chunk_init_filled_gen_time_ms{ 0 };
	int m_num_filled_init_chunks{ 0 };
	int m_num_all_init_chunks{0};


	bool m_k_key_hit{ false };

	glm::ivec3 m_target_chunk_pos{ glm::ivec3() };
	glm::ivec3 m_old_target_chunk_pos{ glm::ivec3() };

	std::queue<ChunkRef> m_cached_chunks;
	std::unordered_map<int, ChunkRef> m_chunk_map;

	std::queue<ChunkRef> m_create_queue;
	std::queue<glm::ivec3> m_delete_queue;

	std::queue<TerrainModEntry> m_terrain_change_queue;

	Opaque_Chunk_Material::Shared m_chunk_opaque_mat;

	Texture::Shared m_diffuse_texture_array;

	bool m_voxel_engine_enabled{ false };

	void Modify_Voxel(glm::ivec3 chunk, std::vector<TerrainMod> values, bool update_neighbor = true, std::unordered_set<int> exclude_global = std::unordered_set<int>());

	ChunkRef get_chunk(glm::ivec3 coord, bool lock = false);

	template<typename T>
	T& get_chunk_comp(glm::ivec3 coord, bool do_lock = false) {
		ChunkRef ref = get_chunk(coord, do_lock);
		std::mutex dummyMutex;
		std::lock_guard<std::mutex> lock(do_lock ? m_process_lock : dummyMutex);
		assert(!ref.chunk_comp.expired());
		return *ref.chunk_comp.lock();
	}

	void initialize_voxel_engine();

	static void AsyncRun(WorldGenController* inst);

	void process();

	void process_additions();

	void process_deletions();

	bool process_batch();

	void process_modifications();

	ChunkRef create_chunk_object();

	void generate_circular();

	void create_chunk_cache();

	bool queue_chunk_create(glm::ivec3 chunk_coord);

	void queue_chunk_delete(glm::ivec3 chunk_coord);

	bool chunk_exists(glm::ivec3 chunk_coord);

	void remove_chunk(glm::ivec3 chunk_coord);


	Standard_Material::Shared standard_mat;
	Mesh::Shared cube_mesh;
	void create_dummy_terrain();

	std::vector<glm::ivec3> get_columns_in_radius(int center_x, int center_z, int radius);

	inline glm::ivec3 worldPosToChunkCoord(const glm::fvec3& position)
	{
		const auto pos = position - glm::vec3(m_chunkMeterSizeX / 2, m_chunkMeterSizeY / 2, m_chunkMeterSizeZ / 2);

		return glm::ivec3(
			std::round(pos.x / (m_chunkMeterSizeX + m_half)),
			std::round(pos.y / (m_chunkMeterSizeY + m_half)),
			std::round(pos.z / (m_chunkMeterSizeZ + m_half))
		);
	}

	inline glm::fvec3 chunkCoordToWorldPos(const glm::ivec3& chunk_coord)
	{
		return glm::fvec3(
			std::round((chunk_coord.x * m_chunkMeterSizeX) - m_half),
			std::round((chunk_coord.y * m_chunkMeterSizeY) - m_half),
			std::round((chunk_coord.z * m_chunkMeterSizeZ) - m_half)
		);
	}

	inline glm::ivec3 voxelToChunk(const glm::ivec3& location)
	{
		int x = floor_to_int(location.x / (float)m_chunk_size_x);
		int y = floor_to_int(location.y / (float)m_chunk_size_y);
		int z = floor_to_int(location.z / (float)m_chunk_size_z);
		return glm::ivec3(x, y, z);
	}

	inline glm::ivec3 chunkToVoxel(const glm::ivec3& location)
	{
		int x = ((location.x) * m_chunk_size_x);
		int y = ((location.y) * m_chunk_size_y);
		int z = ((location.z) * m_chunk_size_z);
		return glm::ivec3(x, y, z);
	}

	inline glm::ivec3 globalToLocalChunkCoord(const glm::ivec3& location)
	{
		glm::ivec3 ChunkCoord = voxelToChunk(location);
		return GlobalToLocalChunkCoord(ChunkCoord, location);
	}

	inline glm::ivec3 globalToLocalChunkCoord(const glm::ivec3& ChunkCoord, const glm::ivec3& location)
	{
		int x = location.x - (ChunkCoord.x * m_chunk_size_x);
		int y = location.y - (ChunkCoord.y * m_chunk_size_y);
		int z = location.z - (ChunkCoord.z * m_chunk_size_z);
		return glm::ivec3(x, y, z);
	}

	inline glm::ivec3 localToGlobalCoord(const glm::ivec3& Chunk, const glm::ivec3& location)
	{
		int x = location.x + (Chunk.x * m_chunk_size_x);
		int y = location.y + (Chunk.y * m_chunk_size_y);
		int z = location.z + (Chunk.z * m_chunk_size_z);
		return glm::ivec3(x, y, z);
	}

	inline glm::fvec3 voxelToWorld(const glm::ivec3& loc)
	{
		float newX = (((loc.x / (float)m_voxelsPerMeter) - m_half));
		float newY = (((loc.y / (float)m_voxelsPerMeter) - m_half));
		float newZ = (((loc.z / (float)m_voxelsPerMeter) - m_half));
		return glm::fvec3(newX, newY, newZ);
	}

	inline glm::ivec3 worldToVoxel(const glm::fvec3& worldPos)
	{
		int x = floor_to_int(((worldPos.x) + m_half) * (float)m_voxelsPerMeter);
		int y = floor_to_int(((worldPos.y) + m_half) * (float)m_voxelsPerMeter);
		int z = floor_to_int(((worldPos.z) + m_half) * (float)m_voxelsPerMeter);
		return glm::ivec3(x, y, z);
	}

	int floor_to_int(float val) {
		return static_cast<int>(std::floor(val));
	}

	inline static const std::string LOG_LOC{ "WORLD_GEN_CONTROLLER" };
};

