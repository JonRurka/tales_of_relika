#include "Block_Type.h"

#include "Material_Processor.h"
#include "Material_Types.h"

std::vector<Block_Type*> Block_Type::m_type_array;
std::unordered_map<int, Block_Type*> Block_Type::m_types;
std::unordered_map<std::string, int> Block_Type::m_name_map;
int Block_Type::m_num_types{0};

int Block_Type::GetTileTexture_Callback(int block_id, int tile_id, uint8_t block_orientation)
{
    if (block_id < 0 || block_id >= m_num_types) {
        return -1;
    }
    Block_Type* block_type = m_type_array[block_id];
    if (block_type == nullptr) {
        // TODO: Probably should throw error.
        return -1;
    }
    return block_type->Get_Tile_Texture(tile_id, block_orientation);
}

bool Block_Type::CanRender_Callback(int block_id)
{
    if(block_id < 0 || block_id >= m_num_types) {
        return false;
    }
    Block_Type* block_type = m_type_array[block_id];
    if (block_type == nullptr) {
        // TODO: Probably should throw error.
        return false;
    }
    return block_type->Can_Render(block_id);
}

void Block_Type::Init()
{
    auto mats = Material_Types::Instance()->Get_Structure_Materials();
    m_num_types = Material_Types::Instance()->Max_ID() + 1;

    m_type_array.clear();
    m_type_array = std::vector<Block_Type*>(m_num_types, nullptr);
    
    Material_Types::Structure_Material air_mat{};
    air_mat.ID = 0;
    air_mat.Material_Name = "air";
    air_mat.IsTransparent = true;
    air_mat.Material_Processor_Name = UNIFORM_PROCESSOR_NAME;
    load_material(air_mat);

    for (auto m : mats) {
        load_material(m);
    }
}

bool Block_Type::Type_Exists(int type_id)
{
    return m_types.contains(type_id);
}

bool Block_Type::Type_Exists(std::string type)
{
    return m_name_map.contains(type);
}

Block_Type* Block_Type::Get_BlockType(int type_id)
{
    if (!Type_Exists(type_id)) {
        return nullptr;
    }
    return m_types[type_id];
}

Block_Type* Block_Type::Get_BlockType(std::string type)
{
    if (!Type_Exists(type)) {
        return nullptr;
    }
    return Get_BlockType(m_name_map[type]);
}

uint32_t Block_Type::Get_Block_Type_ID()
{
    return m_material_type_info.ID;
}

void Block_Type::load_material(Material_Types::Structure_Material mat)
{
    int id = mat.ID;

    if (m_types.contains(id)) {
        return;
    }

    Block_Type* block_type = new Block_Type(mat);
    m_type_array[id] = block_type;
    m_types[id] = block_type;
    m_name_map[Utilities::toLowerCase(mat.Material_Name)] = id;
}

Block_Type::Block_Type(Material_Types::Structure_Material m) 
{
    m_material_type_info = m;

    std::string processor_name = m.Material_Processor_Name;
    if (!Material_Processor::Material_Processor_Exists(processor_name)) {
        Logger::LogWarning(LOG_POS("NEW"), "Material Processor '%s' does not exist... using %s processor.",
            processor_name.c_str(), UNIFORM_PROCESSOR_NAME);
        processor_name = UNIFORM_PROCESSOR_NAME;
    }

    m_material_processor = Material_Processor::Get_Material_Processor(processor_name);
}

int Block_Type::Get_Tile_Texture(int tile_id, uint8_t block_orientation)
{
    int orient_block_id = m_material_processor->Get_Tile_Texture_Index(tile_id, block_orientation);
    int tex_id = m_material_type_info.Textures[orient_block_id].Texture_IDX;
    return tex_id;
}

bool Block_Type::Can_Render(int block_id)
{
    return true; // TODO: change when model types are implemented.
}


