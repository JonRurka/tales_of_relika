#include "Item_Loader.h"

#include "Game_Resources.h"

#include "ini.h"
#include "INIReader.h"

#include "Item_Type.h"



namespace {
    const std::unordered_map<std::string, Item_Type::Item_Category> g_category_names {
        {"None", Item_Type::Item_Category::None },
        {"Structural_Material", Item_Type::Item_Category::Structural_Material},
        {"Terrain_Material", Item_Type::Item_Category::Terrain_Material},
        {"Tool", Item_Type::Item_Category::Tool},
        {"Weapon", Item_Type::Item_Category::Weapon},
        {"Other", Item_Type::Item_Category::Weapon},
        {"Unknown", Item_Type::Item_Category::Unknown},
    };
}

void Item_Loader::Load_Items(std::string resource_file_name)
{
    std::string item_type_ini = Resources::Get_Data_File_String(resource_file_name); //Game_Resources::Data_Files::BLOCK_TYPES

    INIReader reader(item_type_ini.c_str(), item_type_ini.size());
    if (reader.ParseError() < 0) {
        // log error
        Logger::LogError(LOG_POS("Load_Items"), "Failed to load Items from %s", resource_file_name.c_str());
        return;
    }

    std::vector<std::string> sections = reader.Sections();
    for (const auto& elem : sections) {
        Item_Data item{};
        item.Item_Name = elem;
        item.ID = reader.GetInteger(elem, "ID", -1);
        
        std::string cat_str = reader.GetString(elem, "Category", "Unknown");
        assert(g_category_names.contains(cat_str));

        item.Category = (int)g_category_names.at(cat_str);

        if (item.ID == -1) {
            continue;
        }

        m_item_data[item.ID] = item;
        m_item_name_to_id[elem] = item.ID;
    }
}

std::vector<Item_Loader::Item_Data> Item_Loader::Get_Item_Data()
{
    std::vector<Item_Loader::Item_Data> res;
    res.reserve(m_item_data.size());
    for (const auto& elem : m_item_data) {
        res.push_back(elem.second);
    }
    return res;
}
