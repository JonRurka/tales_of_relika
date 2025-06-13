#include "Material_Types.h"

#include "ini.h"
#include "INIReader.h"

#include "Game_Resources.h"

#include <boost/algorithm/string/replace.hpp>

#define MATCH(type, test) (strcmp(type, test) == 0)

#define VOXEL_RESOURCE_DIR "compute::voxelEngine::"
#define HEIGHTMAP_RESOURCE_DIR "compute::heightmapEngine::"


// TODO: Remove annoying duplication
#define VOXEL_RUNTIME_VULKAN 1
#define VOXEL_RUNTIME_OPENCL 2

#define VOXEL_RUNTIME VOXEL_RUNTIME_OPENCL
#define VK_EXT ".comp"
#define CL_EXT ".cl"

#if VOXEL_RUNTIME == VOXEL_RUNTIME_OPENCL
#define EXT CL_EXT
#elif VOXEL_RUNTIME == VOXEL_RUNTIME_VULKAN
#define EXT VK_EXT
#endif


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
        int num_ent = name_split.size();
        for (int i = 1; i < name_split.size(); i++) {
            name += Utilities::toLowerCase(name_split[i]);

            if (i < (num_ent - 1)) {
                name += "-";
            }
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

void Material_Types::Initialize_Materials()
{
    load_terrain_textures();
    set_terrain_gen_macros();
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
    res.reserve(m_terrain_materials.size());
    for (const auto& elem : m_terrain_materials) {
        res.push_back(elem.second);
    }
    return res;
}

std::vector<Material_Types::Structure_Material> Material_Types::Get_Structure_Materials()
{
    std::vector<Structure_Material> res;
    res.reserve(m_structure_materials.size());
    for (const auto& elem : m_structure_materials) {
        res.push_back(elem.second);
    }
    return res;
}

void Material_Types::load_terrain_textures()
{
    std::vector<std::string> diffuse_tex_resources;
    diffuse_tex_resources.reserve(m_terrain_materials.size());

    std::vector<std::string> normal_tex_resources;
    normal_tex_resources.reserve(m_terrain_materials.size());

    int i = 0;
    for (auto& elem : m_terrain_materials) {
        diffuse_tex_resources.push_back(elem.second.Diffuse_Texture_Name);
        m_terrain_materials[elem.second.ID].Texture_IDX = i;
        i++;
    }

    for (const auto& elem : m_terrain_materials) {
        normal_tex_resources.push_back(elem.second.Normals_Texture_Name);
    }

    m_diffuse_texture_array = Texture::Create_Texture2D_Array(diffuse_tex_resources);
    m_normal_texture_array = Texture::Create_Texture2D_Array(normal_tex_resources);
}

void Material_Types::set_terrain_gen_macros()
{
    std::string macro_str = "";

    for (auto& elem : m_terrain_materials) {
        std::string name = elem.second.Material_Name;
        int idx = elem.second.Texture_IDX;
        macro_str += "int " + name + "() { return " + std::to_string(idx) + "; }\n";
    }

    const std::string PROGRAM_SMOOTH_RENDER_CONSTRUCT = "3-smoothrender_gen_iso_normals";
    const std::string PROGRAM_GEN_HEIGHTMAP = "heightmap_gen";

    //std::string heightmap_program_name = HEIGHTMAP_RESOURCE_DIR + PROGRAM_GEN_HEIGHTMAP + EXT;
    std::string construct_program_name = VOXEL_RESOURCE_DIR + PROGRAM_SMOOTH_RENDER_CONSTRUCT + EXT;

    std::vector<char> shader_char = Resources::Get_Shader_bin(construct_program_name);
    std::string shader_str(shader_char.begin(), shader_char.end());

    boost::replace_all(shader_str, "%block_types%", macro_str);

    shader_char = std::vector<char>(shader_str.begin(), shader_str.end());
    Resources::Modify_Shader_Bin(construct_program_name, shader_char);
}

