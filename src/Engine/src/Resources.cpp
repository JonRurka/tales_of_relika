#include "Resources.h"

#include "Utilities.h"
#include "Logger.h"
#include "Texture.h";
#include "Shader.h"
#include "Model.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

Resources* Resources::m_instance{nullptr};

Resources::Resources()
{
#ifdef NDEBUG
    m_mode = LoadMode::Binary;
#else
    m_mode = LoadMode::Filesystem;
#endif

    switch (m_mode) {
    case LoadMode::Filesystem:
        load_shaders_fs();
        load_textures_fs();
        load_models_fs();
        break;
    case LoadMode::Binary:
        load_shaders_binary();
        load_textures_binary();
        load_models_binary();
        break;
    }

    m_instance = this;
    Logger::LogInfo(LOG_POS("NEW"), "Resources Initialized.");
}

Resources::Resources(Resources::LoadMode mode) : Resources()
{
	m_mode = mode;
}

void Resources::load_shader(std::string name)
{
    if (!Has_Shader(name)) {
        Logger::LogError(LOG_POS("Load_Shader"), "Shader not found: %s \n", name.c_str());
        return;
    }

}

void Resources::load_shader(std::vector<std::string> names)
{
    for (const auto& elem : names)
    {
        Load_Shader(elem);
    }
}

void Resources::load_texture(std::string name)
{
    if (!Has_Texture(name)) {
        Logger::LogError(LOG_POS("Load_Texture"), "Texture not found: %s \n", name.c_str());
        return;
    }

    Asset* asset = &m_texture_assets[name];
    if (!asset->loaded) {
        Texture* tex = new Texture(asset->path);
        asset->handle = tex;
        if (tex->Initialized()) {
            asset->loaded = true;
        }
        else {
            Logger::LogError(LOG_POS("Get_Texture"), "Failed to load texture: %s \n", name.c_str());
        }
    }
}

void Resources::load_texture(std::vector<std::string> names)
{
    for (const auto& elem : names)
    {
        Load_Shader(elem);
    }
}

void Resources::load_model(std::string name)
{
    if (!Has_Model(name)) {
        Logger::LogError(LOG_POS("Load_Model"), "Model not found: %s \n", name.c_str());
        return;
    }

    Asset* asset = &m_models_assets[name];
    if (!asset->loaded) {
        Model* model = Model::Load(asset->path);
        asset->handle = model;
        if (model->Initialized()) {
            asset->loaded = true;
        }
        else {
            Logger::LogError(LOG_POS("Load_Model"), "Failed to load Model: %s \n", name.c_str());
        }
    }
}

void Resources::load_model(std::vector<std::string> names)
{
    for (const auto& elem : names)
    {
        Load_Shader(elem);
    }
}

Texture* Resources::get_texture(std::string name)
{
    if (!Has_Texture(name)) {
        Logger::LogError(LOG_POS("Get_Texture"), "Texture not found: %s \n", name.c_str());
        return nullptr;
    }

    Load_Texture(name);

    return static_cast<Texture*>(m_texture_assets[name].handle);
}

std::string Resources::get_shader_file(std::string name)
{
    if (!Has_Shader(name)) {
        Logger::LogError(LOG_POS("Get_Shader_File"), "Shader File not found: %s \n", name.c_str());
        return std::string();
    }

    Load_Shader(name);

    return m_shader_assets[name].path;
}

Model* Resources::get_model(std::string name)
{
    if (!Has_Model(name)) {
        Logger::LogError(LOG_POS("Get_Model"), "Model not found: %s \n", name.c_str());
        return nullptr;
    }

    Load_Model(name);

    return static_cast<Model*>(m_models_assets[name].handle);
}

std::string Resources::Get_Resources_Director()
{
#ifdef NDEBUG
	// release
	return Utilities::Get_Root_Directory() + "resources\\";
#else
	//debug
	return (Utilities::Get_Root_Directory() + "..\\..\\resources\\");
#endif

}

namespace {
    bool hasExtension(const std::string& filename) {
        // Find last occurrence of '.'
        size_t pos = filename.find_last_of('.');

        // If no '.' found, or it's the last character, or it's part of ./ or ../
        if (pos == std::string::npos ||        // No '.' found
            pos == filename.length() - 1 ||    // '.' is last character
            pos == 0 ||                        // '.' is first character
            (pos == 1 && filename[0] == '.'))  // Part of ../
        {
            return false;
        }

        return true;
    }

    void removeLastIf(std::string& str, char ch) {
        if (!str.empty() && str.back() == ch) {
            str.pop_back();
        }
    }

    std::string sep() {
        return Utilities::File_Seperator();
    }
}

void Resources::get_assets_recursively(std::string basePath, std::vector<Asset>& assets, std::string rel_path) {
    removeLastIf(basePath, sep()[0]);

#ifdef _WIN32
    WIN32_FIND_DATA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    char searchPath[MAX_PATH];

    // Construct search path with wildcard
    snprintf(searchPath, MAX_PATH, "%s\\*.*", basePath.c_str());

    hFind = FindFirstFile(searchPath, &ffd);
    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        std::string curr_rel_path = rel_path;
        if (strcmp(ffd.cFileName, ".") == 0 || strcmp(ffd.cFileName, "..") == 0)
            continue;

        //printf("%s\n", ffd.cFileName);

        if (hasExtension(ffd.cFileName)) {

            std::string file_name = std::string(ffd.cFileName);

            std::string rel_file_path = curr_rel_path;
            std::replace(rel_file_path.begin(), rel_file_path.end(), '.', sep()[0]);

            //rel_path += file_name;
            rel_file_path += file_name;

            Asset asset;
            asset.name = curr_rel_path + file_name;
            asset.path = basePath + sep() + file_name;
            asset.loaded = false;
            asset.handle = nullptr;

            assets.push_back(asset);


            //printf("%s, %s \n", asset.path.c_str(), asset.name.c_str());
            //printf("%s\n", asset.path.c_str());
        }
        else {
            curr_rel_path += std::string(ffd.cFileName) + ".";
        }
        

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            char subPath[MAX_PATH];
            snprintf(subPath, MAX_PATH, "%s\\%s", basePath.c_str(), ffd.cFileName);
            get_assets_recursively(subPath, assets, curr_rel_path);
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);

#else
    DIR* dir;
    struct dirent* entry;
    struct stat statbuf;

    // Open directory
    if (!(dir = opendir(basePath.c_str())))
        return;

    // Read directory entries
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        //printf("%s\n", entry->d_name);
        //rel_path += std::string(entry->d_name);

        std::string curr_rel_path = rel_path;

        if (hasExtension(ffd.cFileName)) {

            std::string file_name = std::string(ffd.cFileName);

            std::string rel_file_path = rel_path;
            std::replace(rel_file_path.begin(), rel_file_path.end(), '.', sep()[0]);

            //rel_path += file_name;
            rel_file_path += file_name;

            Asset asset;
            asset.name = rel_path + file_name;
            asset.path = basePath + sep() + file_name;
            asset.loaded = false;
            asset.handle = nullptr;

            assets.push_back(asset);
            //rel_path += std::string(entry->d_name);
            //printf("%s\n", rel_path.c_str());
        }
        else {
            rel_path += std::string(entry->d_name) + ".";
        }

        // Construct full path for stat
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", basePath.c_str(), entry->d_name);

        // Check if it's a directory
        if (stat(fullPath, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
            get_assets_recursively(fullPath, assets, rel_path);
        }
    }

    closedir(dir);
#endif
}


void Resources::load_shaders_fs()
{
	std::string dir = Get_Resources_Director();
	std::string shader_dir = dir + "shaders\\";

    std::vector<Asset> assets;

    get_assets_recursively(shader_dir, assets);

    Logger::LogDebug(LOG_POS("load_shaders_fs"), "Loaded Shaders:");
    for (const auto& a : assets)
    {
        m_shader_assets[a.name] = a;
        Logger::LogDebug(LOG_POS("load_shaders_fs"), "\t - %s", a.name.c_str());
    }
}

void Resources::load_textures_fs()
{
	std::string dir = Get_Resources_Director();
	std::string shader_dir = dir + "textures\\";

    std::vector<Asset> assets;

    get_assets_recursively(shader_dir, assets);

    Logger::LogDebug(LOG_POS("load_textures_fs"), "Loaded Textures:");
    for (const auto& a : assets)
    {
        m_texture_assets[a.name] = a;
        Logger::LogDebug(LOG_POS("load_textures_fs"), "\t - %s", a.name.c_str());
    }
}

void Resources::load_models_fs()
{
	std::string dir = Get_Resources_Director();
	std::string shader_dir = dir + "models\\";

    std::vector<Asset> assets;

    get_assets_recursively(shader_dir, assets);

    for (const auto& a : assets)
    {
        m_models_assets[a.name] = a;
    }
}



void Resources::load_shaders_binary()
{
}

void Resources::load_textures_binary()
{
}

void Resources::load_models_binary()
{
}


