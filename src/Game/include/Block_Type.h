#pragma once

#include "game_engine.h"

#include "Material_Types.h"
#include "Material_Processor.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

class Material_Processor;

class Block_Type {
public:
	typedef std::shared_ptr<Block_Type> Shared;
	typedef std::weak_ptr<Block_Type> Weak;

	inline static std::string Air = "air";
	inline static std::string None = Air;
	inline static std::string Brick = "brick";

	static int GetTileTexture_Callback(int block_id, int tile_id, uint8_t block_orientation);

	static bool CanRender_Callback(int block_id);

	static void Init();

	static bool Type_Exists(int type_id);
	static bool Type_Exists(std::string type);
	static std::shared_ptr<Block_Type> Get_BlockType(int type_id);
	static std::shared_ptr<Block_Type> Get_BlockType(std::string type);

	uint32_t Get_Block_Type_ID();

	int Get_Tile_Texture(int tile_id, uint8_t block_orientation);

	bool Can_Render(int block_id);

private:

	Material_Processor::Shared m_material_processor;
	Material_Types::Structure_Material m_material_type_info{};

	Block_Type(Material_Types::Structure_Material m);


	static std::vector<std::shared_ptr<Block_Type>> m_type_array;
	static std::unordered_map<int, std::shared_ptr<Block_Type>> m_types;
	static std::unordered_map<std::string, int> m_name_map;
	static int m_num_types;

	static void load_material(Material_Types::Structure_Material mat);


	inline static const std::string LOG_LOC{ "BLOCK_TYPE" };
};