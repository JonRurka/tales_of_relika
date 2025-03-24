#pragma once

#include <string>
#include <vector>
#include <unordered_map>

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

	

	static Model* Load(std::string file);

	bool Initialized() { return m_initialized; }

	std::string Name() { return m_name; }

	std::vector<Mesh*> mesh() { return m_mesh; }
	
	std::vector<Texture*> Textures() { return m_textures; }

	std::vector<Model*> Children() { return m_Children; }

	Model* Child(int index) { return m_Children[index]; }

	std::string To_String(int tabs = 0);

private:

	std::string m_name;
	std::vector<Mesh*> m_mesh;
	std::vector<Texture*> m_textures;
	std::vector<Model*> m_Children;

	bool m_initialized{ false };

	static std::unordered_map<size_t, Texture*> m_imported_textures;

	static size_t hash_texture_name(std::string name);

	static Model* Get_Model(std::string dir, aiNode* node, const aiScene* scene);

	static Mesh* Process_Mesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture*> Process_Textures(std::string dir, aiMesh* mesh, const aiScene* scene);

	std::vector<Texture*> loadMaterialTextures(std::string dir, aiMaterial* mat, int type);

	static Texture* Import_Texture(std::string directory, std::string name);
};

