#include "Model.h"

#include "Mesh.h"
#include "Texture.h"

#include <iostream>
#include <tchar.h>
#include <functional>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


std::unordered_map<size_t, Texture*> Model::m_imported_textures;

namespace {
    std::string tab_str(int num) {
        std::string ret;
        for (int i = 0; i < num; i++) {
            ret += "\t";
        }
        return ret;
    }
}



Model* Model::Load(std::string file)
{
    printf("Open model file: %s\n", file.c_str());
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return new Model();
    }

    std::string directory = file.substr(0, file.find_last_of('/'));

    return Get_Model(directory, scene->mRootNode, scene);
}

std::string Model::To_String(int tabs)
{
    std::string str;
    //str += "------------------\n";
    for (int i = 0; i < m_mesh.size(); i++) {
        str += tab_str(tabs) + "Mesh " + std::to_string(i) + ": Verts: " + std::to_string(m_mesh[i]->Vertices().size()) + ", Tris: " + std::to_string(m_mesh[i]->Indices().size()) + "\n";
    }
    for (int i = 0; i < m_textures.size(); i++)
    {
        str += tab_str(tabs) + "Texture: " + m_textures[i]->Name() + "\n";
    }
    for (int i = 0; i < m_Children.size(); i++) {
        str += m_Children[i]->To_String(tabs + 1);
    }
    //str += "------------------\n";
    return str;
}

Model* Model::Get_Model(std::string directory, aiNode* node, const aiScene* scene)
{
    Model* model = new Model();

    std::string name = std::string(node->mName.C_Str());

    model->m_name = name;

    for (int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh* g_mesh = Process_Mesh(ai_mesh, scene);
        g_mesh->Name(name);
        model->m_mesh.push_back(g_mesh);
        model->Process_Textures(directory, ai_mesh, scene);
    }

    for (int i = 0; i < node->mNumChildren; i++) {
        model->m_Children.push_back(Get_Model(directory, node->mChildren[i], scene));
    }

    model->m_initialized = true;

    return model;
}

Mesh* Model::Process_Mesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;
    
    for (int i = 0; i < mesh->mNumVertices; i++) {
        glm::vec3 vert;
        glm::vec3 norm;
        glm::vec2 t_coord;

        vert.x = mesh->mVertices[i].x;
        vert.y = mesh->mVertices[i].y;
        vert.z = mesh->mVertices[i].z;

        norm.x = mesh->mNormals[i].x;
        norm.y = mesh->mNormals[i].y;
        norm.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0]) {
            t_coord.x = mesh->mTextureCoords[0][i].x;
            t_coord.y = mesh->mTextureCoords[0][i].y;
        }
        else {
            t_coord = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vert);
        normals.push_back(norm);
        texCoords.push_back(t_coord);
    }

    printf("Added vertices to model Mesh: %i\n", (int)vertices.size());

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }


    Mesh* res = new Mesh();
    res->Vertices(vertices);
    res->Normals(normals);
    res->TexCoords(texCoords);
    res->Indices(indices);

    res->Recenter();

    return res;
}

std::vector<Texture*> Model::Process_Textures(std::string dir, aiMesh* mesh, const aiScene* scene)
{
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture*> diffuse_texas = loadMaterialTextures(dir, material, aiTextureType_DIFFUSE);
        std::vector<Texture*> spec_texas = loadMaterialTextures(dir, material, aiTextureType_SPECULAR);
        std::vector<Texture*> norm_texas = loadMaterialTextures(dir, material, aiTextureType_NORMALS);

        m_textures.insert(m_textures.end(), diffuse_texas.begin(), diffuse_texas.end());
        m_textures.insert(m_textures.end(), spec_texas.begin(), spec_texas.end());
        m_textures.insert(m_textures.end(), norm_texas.begin(), norm_texas.end());
    }

    return std::vector<Texture*>();
}

std::vector<Texture*> Model::loadMaterialTextures(std::string dir, aiMaterial* mat, int type)
{
    std::vector<Texture*> res;
    for (unsigned int i = 0; i < mat->GetTextureCount((aiTextureType)type); i++)
    {
        aiString str;
        mat->GetTexture((aiTextureType)type, i, &str);
        Texture* tex = Import_Texture(dir, std::string(str.C_Str()));
        if ((aiTextureType)type == aiTextureType_DIFFUSE) {

        }
        switch ((aiTextureType)type)
        {
            case aiTextureType_DIFFUSE:
                tex->type(Texture::Type::DIFFUSE);
                break;
            case aiTextureType_SPECULAR:
                tex->type(Texture::Type::SPECULAR);
                break;
            case aiTextureType_NORMALS:
                tex->type(Texture::Type::NORMAL);
                break;
            default:
                tex->type(Texture::Type::OTHER);
                break;
        }
        res.push_back(tex);
    }

    return res;
}

Texture* Model::Import_Texture(std::string directory, std::string name)
{
    std::string full_path = directory + name;
    size_t tex_hash = hash_texture_name(full_path);

    if (m_imported_textures.contains(tex_hash)) 
    {
        return m_imported_textures[tex_hash];
    }

    Texture* tex = new Texture(full_path);
    m_imported_textures[tex_hash] = tex;
    return tex;
}

size_t  Model::hash_texture_name(std::string name)
{
    std::hash<std::string> hasher;
    return hasher(name);
}

