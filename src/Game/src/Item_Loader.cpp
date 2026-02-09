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

    const std::unordered_set<std::string> ignore{
            "ID", "Category", "Held_Mesh", "held_Mesh_Tex", "held_Mesh_Normal_Tex"
    };

    Item_Data empty_item{};
    empty_item.Item_Name = "empty";
    empty_item.ID = 0;
    empty_item.Category = (int)Item_Type::Item_Category::None;
    m_item_data[empty_item.ID] = empty_item;
    m_item_name_to_id[empty_item.Item_Name] = empty_item.ID;

    std::vector<std::string> sections = reader.Sections();
    for (const auto& elem : sections) {
        Item_Data item{};
        item.Item_Name = elem;
        item.ID = reader.GetInteger(elem, "ID", -1);
        
        std::string cat_str = reader.GetString(elem, "Category", "Unknown");
        assert(g_category_names.contains(cat_str));

        item.Held_Mesh = reader.GetString(elem, "Held_Mesh", "Unknown");
        item.held_Mesh_Tex = reader.GetString(elem, "held_Mesh_Tex", "Unknown");
        item.held_Mesh_Normal_Tex = reader.GetString(elem, "held_Mesh_Normal_Tex", "Unknown");

        item.Category = (int)g_category_names.at(cat_str);

        if (item.ID == -1) {
            continue;
        }

        m_item_data[item.ID] = item;
        m_item_name_to_id[elem] = item.ID;

        
        auto all_sec_keys = reader.Keys(elem);
        for (const auto& k : all_sec_keys) {
            if (ignore.contains(k)) {
                continue;
            }
            item.Other_Attributes[k] = reader.GetString(elem, k, "");
        }
    }

    m_item_cache.clear();
    m_item_cache.reserve(m_item_data.size());
    for (const auto& elem : m_item_data) {
        m_item_cache.push_back(elem.second);
    }
}

const std::vector<Item_Loader::Item_Data>& Item_Loader::Get_Item_Data()
{
    return m_item_cache;
}
