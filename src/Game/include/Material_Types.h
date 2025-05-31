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
		std::string Material_Name;
		std::string Diffuse_Texture_Name;
		std::string Normals_Texture_Name;
		glm::fvec3 Blend_Color;
	};

	struct Structure_Material {
		int ID;
		std::string Material_Name;
	};

	Material_Types();

	void Load_Materials(std::string resource_file_name);

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

	static Material_Types* Instance() { return m_instance; }

private:

	static Material_Types* m_instance;

	std::unordered_map<int, Terrain_Material> m_terrain_materials;
	std::unordered_map<std::string, int> m_terrain_mat_to_id;
	std::unordered_map<int, Structure_Material> m_structure_materials;
	std::unordered_map<std::string, int> m_structure_mat_to_id;

};