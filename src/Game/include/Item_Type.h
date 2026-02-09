#pragma once

#include <string>
#include <memory>

#include "Item_Loader.h"

class Item_Type {
public:

	enum class Item_Category : int {
		None,
		Structural_Material,
		Terrain_Material,
		Tool,
		Weapon,
		Other,
		Unknown,
	};

	typedef std::shared_ptr<Item_Type> Shared;
	typedef std::weak_ptr<Item_Type> Weak;

	


	static void Init();

	static bool Type_Exists(int type_id);
	static bool Type_Exists(std::string type);
	static Item_Type::Shared Get_ItemType(int type_id);
	static Item_Type::Shared Get_ItemType(std::string type);

	std::string Get_Name() { return m_name; }
	int Get_ID() { return m_ID;}
	int Max_Stack_Size() { return m_stack_size; }
	Item_Category Get_Category() { return m_category; }

	Item_Type(Item_Loader::Item_Data data);

private:

	int m_ID{ 0 };
	std::string m_name;
	int m_stack_size{ 0 };
	Item_Category m_category{ Item_Category::None };

	static std::unordered_map<int, Item_Type::Shared> m_types;
	static std::unordered_map<std::string, Item_Type::Shared> m_name_map;

	static void load_item(Item_Loader::Item_Data data);

	
public:

	static Item_Type::Shared Empty();

	// Terrain
	static Item_Type::Shared Grass() { return m_name_map.at("grass"); }
	static Item_Type::Shared Dirt() { return m_name_map.at("girt"); }
	static Item_Type::Shared Stone() { return m_name_map.at("stone"); }

	// Structural
	static Item_Type::Shared Brick() { return m_name_map.at("brick"); }

};