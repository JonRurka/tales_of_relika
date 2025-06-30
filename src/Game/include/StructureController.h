#pragma once

#include "game_engine.h"

#include "shared_structures.h"


#include <unordered_map>
#include <vector>
#include <queue>

using namespace VoxelEngine;

class StructureChunk;

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

	bool Chunk_Exists(glm::ivec3 chunk_coord) { return chunk_exists(chunk_coord); };
	
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

	static StructureController* m_Instance;

	std::queue<ChunkRef> m_cached_chunks;
	std::unordered_map<int, ChunkRef> m_chunk_map;

	std::queue<ChunkRef> m_create_queue;
	std::queue<glm::ivec3> m_delete_queue;

	std::queue<TerrainModEntry> m_terrain_change_queue;

	void initialize_voxel_engine();

	void process_additions();

	void process_deletions();

	bool queue_chunk_create(glm::ivec3 chunk_coord);

	void queue_chunk_delete(glm::ivec3 chunk_coord);

	bool chunk_exists(glm::ivec3 chunk_coord);

	void remove_chunk(glm::ivec3 chunk_coord);


};
