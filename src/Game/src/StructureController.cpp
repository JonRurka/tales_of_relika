#include "StructureController.h"

#include "CubeVoxelBuilder.h"

StructureController* StructureController::m_Instance{nullptr};

StructureController::StructureMod::StructureMod(glm::ivec3 voxel, uint32_t type)
{
	Voxel = voxel;
	Orientation = CubeVoxelBuilder::Get_Block_Orientation(type);
	Type = CubeVoxelBuilder::Get_Block_Type(type);
	Change_Type = false;
}

void StructureController::Init()
{
	


}

void StructureController::Update(float dt)
{



}

void StructureController::initialize_voxel_engine()
{
}

void StructureController::process_additions()
{
}

void StructureController::process_deletions()
{
}

bool StructureController::queue_chunk_create(glm::ivec3 chunk_coord)
{
	return false;
}

void StructureController::queue_chunk_delete(glm::ivec3 chunk_coord)
{
}

bool StructureController::chunk_exists(glm::ivec3 chunk_coord)
{
	return false;
}

void StructureController::remove_chunk(glm::ivec3 chunk_coord)
{
}
