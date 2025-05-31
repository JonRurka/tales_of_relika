#include "Material_Types.h"

#include "ini.h"
#include "INIReader.h"

#include "Game_Resources.h"

#define MATCH(type, test) (strcmp(type, test) == 0)


Material_Types* Material_Types::m_instance{nullptr};

namespace {

    glm::fvec3 Parse_Color(std::string val) {

        std::vector<std::string> vec_split = Utilities::Split(val, ",");

        if (vec_split.size() == 3) {
            // error
            return glm::fvec3(0.0);
        }

        return glm::fvec3(
            std::stof(vec_split[0]), 
            std::stof(vec_split[1]), 
            std::stof(vec_split[2])
        );
    }

    Material_Types::Type Get_Type(std::string val) {
        std::vector<std::string> name_split = Utilities::Split(val, "-");
        std::string type_name = Utilities::toLowerCase(name_split[0]);

        if (MATCH("terrain", type_name.c_str())) {
            return Material_Types::Type::Terrain;
        }
        else if (MATCH("structure", type_name.c_str())) {
            return Material_Types::Type::Structure;
        }
        else {
            return Material_Types::Type::Terrain;
        }
    }

    std::string Get_Name(std::string val) {
        std::vector<std::string> name_split = Utilities::Split(val, "-");
        std::string name = "";
        for (int i = 1; i < name_split.size(); i++) {
            name += "-" + Utilities::toLowerCase(name_split[i]);
        }
        return name;
    }
}


Material_Types::Material_Types()
{
    m_instance = this;
}

void Material_Types::Load_Materials(std::string resource_file_name)
{
    std::string block_type_ini = Resources::Get_Data_File_String(Game_Resources::Data_Files::BLOCK_TYPES);

    INIReader reader(block_type_ini.c_str(), block_type_ini.size());
    if (reader.ParseError() < 0) {
        // log error
        return;
    }

    std::vector<std::string> sections = reader.Sections();
    for (const auto& elem : sections) {
        Type type = Get_Type(elem);
        std::string mat_name = Get_Name(elem);

        if (type == Type::Terrain) {

            if (m_terrain_mat_to_id.contains(mat_name))
                continue;

            Terrain_Material mat{};
            mat.Material_Name = mat_name;
            mat.ID = reader.GetInteger(elem, "ID", -1);
            mat.Diffuse_Texture_Name = reader.GetString(elem, "texture_diffuse", "");
            mat.Normals_Texture_Name = reader.GetString(elem, "texture_normal", "");
            mat.Blend_Color = Parse_Color(reader.GetString(elem, "blend_color", "0, 0, 0"));

            if (mat.ID == -1) {
                continue;
            }
            m_terrain_materials[mat.ID] = mat;
            m_terrain_mat_to_id[mat_name] = mat.ID;
        }
        else if (type == Type::Structure) {

            if (m_structure_mat_to_id.contains(mat_name))
                continue;

            Structure_Material mat{};
            mat.Material_Name = mat_name;
            mat.ID = reader.GetInteger(elem, "ID", -1);




            if (mat.ID == -1) {
                continue;
            }
            m_structure_materials[mat.ID] = mat;
            m_structure_mat_to_id[mat_name] = mat.ID;
        }
    }


}

int Material_Types::Terrain_Material_ID(std::string name)
{
    if (!Has_Terrain_Material(name)) {
        return -1;
    }
    return m_terrain_mat_to_id[name];
}

int Material_Types::Structure_Material_ID(std::string name)
{
    if (!Has_Structure_Material(name)) {
        return -1;
    }
    return m_structure_mat_to_id[name];
}

bool Material_Types::Has_Terrain_Material(std::string name)
{
    return m_terrain_mat_to_id.contains(name);
}

bool Material_Types::Has_Structure_Material(std::string name)
{
    return m_structure_mat_to_id.contains(name);
}

Material_Types::Terrain_Material Material_Types::Get_Terrain_Material(std::string name)
{
    if (!Has_Terrain_Material(name)) {
        return Terrain_Material{ -1};
    }
    return Get_Terrain_Material(Terrain_Material_ID(name));
}

Material_Types::Terrain_Material Material_Types::Get_Terrain_Material(int ID)
{
    if (!m_terrain_materials.contains(ID)) {
        return Terrain_Material{ -1 };
    }
    return m_terrain_materials[ID];
}

Material_Types::Structure_Material Material_Types::Get_Structure_Material(std::string name)
{
    if (!Has_Structure_Material(name)) {
        return Structure_Material{ -1 };
    }
    return Get_Structure_Material(Structure_Material_ID(name));
}

Material_Types::Structure_Material Material_Types::Get_Structure_Material(int ID)
{
    if (!m_structure_materials.contains(ID)) {
        return Structure_Material{ -1 };
    }
    return m_structure_materials[ID];
}

std::vector<Material_Types::Terrain_Material> Material_Types::Get_Terrain_Materials()
{
    std::vector<Terrain_Material> res;
    for (const auto& elem : m_structure_materials) {

    }
    return res;
}

std::vector<Material_Types::Structure_Material> Material_Types::Get_Structure_Materials()
{
    std::vector<Structure_Material> res;

    return res;
}

