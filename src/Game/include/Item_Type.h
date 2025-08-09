#pragma once

#include <string>
#include <memory>

#include "Item_Loader.h"

class Item_Type {
public:
	typedef std::shared_ptr<Item_Type> Shared;
	typedef std::weak_ptr<Item_Type> Weak;

	static Item_Type::Shared Empty();

	static void Init();

	static bool Type_Exists(int type_id);
	static bool Type_Exists(std::string type);
	static Item_Type::Shared Get_ItemType(int type_id);
	static Item_Type::Shared Get_ItemType(std::string type);

	std::string Get_Name() { return m_name; }
	int Get_ID() { return m_ID;}
	int Max_Stack_Size() { return m_stack_size; }

	Item_Type(Item_Loader::Item_Data data);

private:

	int m_ID{ 0 };
	std::string m_name;
	int m_stack_size{ 0 };

	static std::unordered_map<int, Item_Type::Shared> m_types;
	static std::unordered_map<std::string, Item_Type::Shared> m_name_map;

	static void load_item(Item_Loader::Item_Data data);

	
	

};