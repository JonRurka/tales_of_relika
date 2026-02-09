#include "Held_Empty_Action.h"

#include "WorldGenController.h"

Held_Empty_Action::Held_Empty_Action()
{
	Init_ISO_Sampler();
}

void Held_Empty_Action::Use_Left(Inventory_Item& item, const UseInfo& hit)
{
	glm::ivec3 hit_voxel = Hit_Closest_voxel(hit);
	glm::ivec3 chunk = WorldGenController::VoxelToChunk(hit_voxel);
	glm::ivec3 local_voxel = WorldGenController::GlobalToLocalChunkCoord(chunk, hit_voxel);

	float iso = ISO_Sample().Get_ISO(chunk, local_voxel);

	std::vector<WorldGenController::TerrainMod> changes;
	WorldGenController::TerrainMod mod(hit_voxel, iso + 0.2f);
	changes.push_back(mod);

	WorldGenController::Instance()->Modify_Voxel(changes);
}

void Held_Empty_Action::Use_Right(Inventory_Item& item, const UseInfo& hit)
{

}
