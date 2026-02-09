#include "Item_Type.h"

#include "Item_Loader.h"

std::unordered_map<int, Item_Type::Shared> Item_Type::m_types;
std::unordered_map<std::string, Item_Type::Shared> Item_Type::m_name_map;

Item_Type::Shared Item_Type::Empty()
{
    return m_types[0];
}

void Item_Type::Init()
{
	auto data = Item_Loader::Instance().Get_Item_Data();

	m_types.clear();
    m_name_map.clear();


    Item_Loader::Item_Data empty_item{};
    empty_item.ID = 0;
    empty_item.Item_Name = "empty";
    empty_item.Category = (int)Item_Category::None;
    //load_item(empty_item);

	for (auto m : data) {
		load_item(m);
	}

}

bool Item_Type::Type_Exists(int type_id) 
{
    return m_types.contains(type_id);
}

bool Item_Type::Type_Exists(std::string type) 
{
    return m_name_map.contains(type);
}

Item_Type::Shared Item_Type::Get_ItemType(int type_id) 
{
    if (!Type_Exists(type_id)) {
        return nullptr;
    }
    return m_types[type_id];
}

Item_Type::Shared Item_Type::Get_ItemType(std::string type) 
{
    if (!Type_Exists(type)) {
        return nullptr;
    }
    return m_name_map[type];
}


void Item_Type::load_item(Item_Loader::Item_Data data)
{
    int id = data.ID;

    if (m_types.contains(id)) {
        return;
    }

    Item_Type::Shared block_type = std::make_shared<Item_Type>(data);
    m_types[id] = block_type;
    m_name_map[data.Item_Name] = block_type; //Utilities::toLowerCase(data.Item_Name)
}

Item_Type::Item_Type(Item_Loader::Item_Data data)
{
    m_ID = data.ID;
    m_name = data.Item_Name;
    m_category = (Item_Category)data.Category;
}

