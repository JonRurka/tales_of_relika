#pragma once

#include "game_engine.h"

class Item_Loader {
public:

	struct Item_Data {
		int ID;
		std::string Item_Name;
	};

	static Item_Loader* Instance() { return m_instance; }

	Item_Loader();

	void Load_Items(std::string resource_file_name);

	std::vector<Item_Data> Get_Item_Data();

private:

	static Item_Loader* m_instance;

	std::unordered_map<int, Item_Data> m_item_data;
	std::unordered_map<std::string, int> m_item_name_to_id;

};