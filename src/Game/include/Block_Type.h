#pragma once

#include "game_engine.h"

#include "Material_Types.h"

#include <vector>
#include <unordered_map>

class Material_Processor;

class Block_Type {
public:

	static int GetTileTexture_Callback(int block_id, int tile_id, uint8_t block_orientation);

	static bool CanRender_Callback(int block_id);

	static void Init();

	int Get_Tile_Texture(int tile_id, uint8_t block_orientation);

	bool Can_Render(int block_id);

private:

	Material_Processor* m_material_processor{ nullptr };
	Material_Types::Structure_Material m_material_type_info{};

	Block_Type(Material_Types::Structure_Material m);


	static std::vector<Block_Type*> m_type_array;
	static std::unordered_map<int, Block_Type*> m_types;
	static int m_num_types;

	static void load_material(Material_Types::Structure_Material mat);


	inline static const std::string LOG_LOC{ "BLOCK_TYPE" };
};