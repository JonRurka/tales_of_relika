#pragma once

#include "game_engine.h"

class Item_Loader {
public:

	struct Item_Data {
		int ID;
		std::string Item_Name;
		int Category;

	};

	static Item_Loader& Instance() 
	{ 
		static Item_Loader inst;
		return inst; 
	}

	void Load_Items(std::string resource_file_name);

	std::vector<Item_Data> Get_Item_Data();

private:
	Item_Loader() = default;

	std::unordered_map<int, Item_Data> m_item_data;
	std::unordered_map<std::string, int> m_item_name_to_id;

	inline static const std::string LOG_LOC{ "ITEM_LOADER" };
};