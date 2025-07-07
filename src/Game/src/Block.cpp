#include "Block.h"

#include "StructureController.h"
#include "StructureDataStorage.h"
#include "CubeVoxelBuilder.h"
#include "Block_Type.h"

using namespace VoxelEngine;

Block Block::Get_Block(glm::ivec3 coord)
{
    glm::ivec3 chunk = StructureController::VoxelToChunk(coord);
    glm::ivec3 voxel_local = StructureController::GlobalToLocalChunkCoord(chunk, coord);

    uint32_t type_data = 0;
    if (StructureController::Instance()->Chunk_Exists(chunk))
    {
        StructureDataStorage* data_store = StructureController::Instance()->Get_Data_Storage();
        voxel_local.x += data_store->Grid_Offset();
        voxel_local.y += data_store->Grid_Offset();
        voxel_local.z += data_store->Grid_Offset();
        type_data = data_store->Get_Data(chunk, voxel_local);
    }

    uint32_t block_type = CubeVoxelBuilder::Get_Block_Type(type_data);

    Block_Type* block_type_obj = Block_Type::Get_BlockType(block_type);

    return Block(coord, block_type_obj);
}

void Block::Set_Type(uint32_t type_id)
{
    if (!Block_Type::Type_Exists(type_id)) {
        return;
    }
    Set_Type(Block_Type::Get_BlockType(type_id));
}

void Block::Set_Type(std::string type_name)
{
    if (!Block_Type::Type_Exists(type_name)) {
        return;
    }
    Set_Type(Block_Type::Get_BlockType(type_name));
}

void Block::Set_Type(Block_Type* type_object)
{
    if (type_object == nullptr) {
        return;
    }

    m_block_type = type_object;
    uint32_t type_id = m_block_type->Get_Block_Type_ID();
    uint32_t block_data = CubeVoxelBuilder::New_Block_Data(type_id, 0);

    glm::ivec3 chunk = StructureController::VoxelToChunk(m_global_coord);
    glm::ivec3 voxel_local = StructureController::GlobalToLocalChunkCoord(chunk, m_global_coord);

    if (!StructureController::Instance()->Chunk_Exists(chunk)) {
        StructureController::StructureMod new_block(voxel_local, block_data);
        std::vector<StructureController::StructureMod> mod;
        mod.push_back(new_block);
        StructureController::Instance()->Spawn_Chunk(chunk, mod);
        Logger::LogDebug(LOG_POS("Set_Type"), "Spawning new chunk...");
    }
    else {
        //StructureController::StructureMod new_block(voxel_coord, block_type_raw);
        StructureController::Instance()->Modify_Voxel_Type(m_global_coord, block_data);
        Logger::LogDebug(LOG_POS("Set_Type"), "Placing Block");

    }


}


