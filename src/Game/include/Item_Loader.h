#pragma once

#include "game_engine.h"

class Item_Loader {
public:

	struct Item_Data {
		int ID;
		std::string Item_Name;
		int Category;

		std::string Held_Mesh;
		std::string held_Mesh_Tex;
		std::string held_Mesh_Normal_Tex;

		std::unordered_map<std::string, std::string> Other_Attributes;
	};

	static Item_Loader& Instance() 
	{ 
		static Item_Loader inst;
		return inst; 
	}

	void Load_Items(std::string resource_file_name);

	const std::vector<Item_Data>& Get_Item_Data();

private:
	Item_Loader() = default;

	std::unordered_map<int, Item_Data> m_item_data;
	std::unordered_map<std::string, int> m_item_name_to_id;

	std::vector<Item_Loader::Item_Data> m_item_cache;

	inline static const std::string LOG_LOC{ "ITEM_LOADER" };
};