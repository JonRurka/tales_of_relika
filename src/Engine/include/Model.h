#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>

class Mesh;
class Texture;
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

class Model
{
public:

	

	static std::shared_ptr<Model> Load(std::string file);

	static std::shared_ptr<Model> Load(std::string resource_name, std::vector<char> data);

	bool Initialized() { return m_initialized; }

	std::string Name() { return m_name; }

	std::vector<std::shared_ptr<Mesh>> mesh() { return m_mesh; }
	
	std::vector<std::shared_ptr<Texture>> Textures() { return m_textures; }

	std::vector<std::shared_ptr<Model>> Children() { return m_Children; }

	std::shared_ptr<Model> Child(int index) { return m_Children[index]; }

	std::string To_String(int tabs = 0);

private:

	std::string m_name;
	std::vector<std::shared_ptr<Mesh>> m_mesh;
	std::vector<std::shared_ptr<Texture>> m_textures;
	std::vector<std::shared_ptr<Model>> m_Children;

	bool m_initialized{ false };

	static std::unordered_map<size_t, std::shared_ptr<Texture>> m_imported_textures;

	static size_t hash_texture_name(std::string name);

	static std::shared_ptr<Model> Get_Model(std::string dir, aiNode* node, const aiScene* scene);

	static std::shared_ptr<Mesh> Process_Mesh(aiMesh* mesh, const aiScene* scene);

	std::vector<std::shared_ptr<Texture>> Process_Textures(std::string dir, aiMesh* mesh, const aiScene* scene);

	std::vector<std::shared_ptr<Texture>> loadMaterialTextures(std::string dir, aiMaterial* mat, int type);

	static std::shared_ptr<Texture> Import_Texture(std::string directory, std::string name);

	inline static const std::string LOG_LOC{ "MODEL" };
};

