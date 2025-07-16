#include "Item_Loader.h"

#include "Game_Resources.h"

#include "ini.h"
#include "INIReader.h"

Item_Loader* Item_Loader::m_instance{nullptr};

Item_Loader::Item_Loader() {
    m_instance = this;
}

void Item_Loader::Load_Items(std::string resource_file_name)
{
    std::string item_type_ini = Resources::Get_Data_File_String(Game_Resources::Data_Files::BLOCK_TYPES);

    INIReader reader(item_type_ini.c_str(), item_type_ini.size());
    if (reader.ParseError() < 0) {
        // log error
        return;
    }

    std::vector<std::string> sections = reader.Sections();
    for (const auto& elem : sections) {
        Item_Data item{};
        item.Item_Name = elem;
        item.ID = reader.GetInteger(elem, "ID", -1);

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
