#pragma once

#include "game_engine.h"

class Material_Types {
public:

	enum class Type {
		Terrain,
		Structure
	};

	struct Terrain_Material {
		int ID;
		int Texture_IDX;
		bool IsTransparent;
		std::string Material_Name;
		std::string Diffuse_Texture_Name;
		std::string Normals_Texture_Name;
		glm::fvec3 Blend_Color;
	};

	struct Structure_Material_Texture {
		int Texture_IDX;
		std::string Diffuse_Texture_Name;
		std::string Normals_Texture_Name;
	};

	struct Structure_Material {
		int ID;
		int Internal_ID;
		std::string Material_Name;
		bool IsTransparent;

		std::vector<Structure_Material_Texture> Textures;
		std::string Material_Processor_Name;
	};

	Material_Types();

	void Load_Materials(std::string resource_file_name);

	void Initialize_Materials(bool load_textures = true);

	int Terrain_Material_ID(std::string name);

	int Structure_Material_ID(std::string name);

	bool Has_Terrain_Material(std::string name);

	bool Has_Structure_Material(std::string name);

	Terrain_Material Get_Terrain_Material(std::string name);
	Terrain_Material Get_Terrain_Material(int ID);

	Structure_Material Get_Structure_Material(std::string name);
	Structure_Material Get_Structure_Material(int ID);

	std::vector<Terrain_Material> Get_Terrain_Materials();
	std::vector<Structure_Material> Get_Structure_Materials();

	Texture* Terrain_Diffuse_Texture_Array() { return m_terrain_diffuse_texture_array; }
	Texture* Terrain_Normal_Texture_Array() { return m_terrain_normal_texture_array; }

	Texture* Structure_Diffuse_Texture_Array() { return m_structure_diffuse_texture_array; }
	Texture* Structure_Normal_Texture_Array() { return m_structure_normal_texture_array; }

	int Max_ID() { return m_max_id; }

	static Material_Types* Instance() { return m_instance; }

private:

	static Material_Types* m_instance;

	std::unordered_map<int, Terrain_Material> m_terrain_materials;
	std::unordered_map<std::string, int> m_terrain_mat_to_id;
	std::unordered_map<int, Structure_Material> m_structure_materials;
	std::unordered_map<std::string, int> m_structure_mat_to_id;

	Texture* m_terrain_diffuse_texture_array{ nullptr };
	Texture* m_terrain_normal_texture_array{ nullptr };

	Texture* m_structure_diffuse_texture_array{ nullptr };
	Texture* m_structure_normal_texture_array{ nullptr };

	int m_max_id{ 0 };

	void load_terrain_textures();

	void load_structure_textures();

	void set_terrain_gen_macros();

	void set_shader_macros(std::string program_name, std::string macro_str);

};