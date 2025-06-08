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

#include <nlohmann/json.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

using nlohmann::json;

#define LOAD_MODE_FS 1
#define LOAD_MODE_BIN 2

#define LOAD_MODE LOAD_MODE_BIN

Resources* Resources::m_instance{nullptr};

namespace {
    std::string pad_int(int value, int width) {
        std::ostringstream oss;
        oss << std::setw(width) << std::setfill('0') << value;
        return oss.str();
    }

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


Resources::Resources()
{
    if (LOAD_MODE == LOAD_MODE_FS) {
        m_mode = LoadMode::Filesystem;
    }
    else if (LOAD_MODE == LOAD_MODE_BIN) {
        m_mode = LoadMode::Binary;
    }


    switch (m_mode) {
    case LoadMode::Filesystem:
        load_shaders_fs();
        load_textures_fs();
        load_models_fs();
        load_data_fs();
        break;
    case LoadMode::Binary:
        load_shaders_binary();
        load_textures_binary();
        load_models_binary();
        load_data_binary();
        break;
    }

    m_instance = this;
    Logger::LogInfo(LOG_POS("NEW"), "Resources Initialized.");
}

Resources::Resources(Resources::LoadMode mode) : Resources()
{
	m_mode = mode;
}

std::vector<std::string> Resources::Get_Data_Resource_List()
{
    std::vector<std::string> res;
    res.reserve(m_instance->m_data_assets.size());
    for (const auto& pair : m_instance->m_data_assets)
    {
        res.push_back(pair.first);
    }
    return res;
}

std::vector<std::string> Resources::Get_Data_Resource_List(std::string extension)
{
    std::vector<std::string> res;
    res.reserve(m_instance->m_data_assets.size());
    for (const auto& pair : m_instance->m_data_assets)
    {
        if (Utilities::getFileExtension(pair.first) == extension) {
            res.push_back(pair.first);
        }
    }
    return res;
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
        load_shader(elem);
    }
}

void Resources::load_texture(std::string name, bool flip)
{
    if (!Has_Texture(name)) {
        Logger::LogError(LOG_POS("Load_Texture"), "Texture not found: %s \n", name.c_str());
        return;
    }

    Asset* asset = &m_texture_assets[name];
    if (asset->loaded)
    {
        return;
    }

    if (LOAD_MODE == LOAD_MODE_FS)
    {
        Texture* tex = new Texture(asset->path, flip);
        asset->handle = tex;
        asset->data = tex->Data();
        if (tex->Initialized()) {
            asset->loaded = true;
        }
        else {
            Logger::LogError(LOG_POS("Get_Texture"), "Failed to load texture: %s \n", name.c_str());
        }
        
    }
    else if (LOAD_MODE == LOAD_MODE_BIN) 
    {
        load_pack_data(asset, PackType::Texture_Type);
        char* data_ptr = (char*)asset->data;
        size_t data_size = asset->data_size;
        Texture* tex = new Texture(name, std::vector<char>(data_ptr, data_ptr + data_size), flip);
        asset->handle = tex;
        asset->data = tex->Data();
        if (tex->Initialized()) {
            asset->loaded = true;
        }
        else {
            Logger::LogError(LOG_POS("Get_Texture"), "Failed to load texture: %s \n", name.c_str());
        }
    }
}

void Resources::load_texture(std::vector<std::string> names, bool flip)
{
    for (const auto& elem : names)
    {
        load_texture(elem, flip);
    }
}

void Resources::load_model(std::string name)
{
    if (!Has_Model(name)) {
        Logger::LogError(LOG_POS("load_model"), "Model not found: %s \n", name.c_str());
        return;
    }

    Asset* asset = &m_models_assets[name];
    if (asset->loaded) {
        return;
    }

    if (LOAD_MODE == LOAD_MODE_FS)
    {
        Model* model = Model::Load(asset->path);
        asset->handle = model;
        if (model->Initialized()) {
            asset->loaded = true;
        }
        else {
            Logger::LogError(LOG_POS("load_model"), "Failed to load Model: %s \n", name.c_str());
        }
    }
    else if (LOAD_MODE == LOAD_MODE_BIN)
    {
        load_pack_data(asset, PackType::Model_Type);
        char* data_ptr = (char*)asset->data;
        size_t data_size = asset->data_size;
        Model* model = Model::Load(name, std::vector<char>(data_ptr, data_ptr + data_size));
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
        load_model(elem);
    }
}

void Resources::load_data_file(std::string name)
{
    if (!Has_Data_File(name)) {
        Logger::LogError(LOG_POS("load_data_file"), "Data File not found: %s \n", name.c_str());
        return;
    }

    Asset* asset = &m_data_assets[name];
    if (asset->loaded) {
        return;
    }

    if (LOAD_MODE == LOAD_MODE_FS) 
    {
        // ...
    }
    else {
        load_pack_data(asset, PackType::Data_File_Type);
        //char* data_ptr = (char*)asset->data;
        //size_t data_size = asset->data_size;
    }
}

void Resources::load_data_file(std::vector<std::string> names)
{
    for (const auto& elem : names)
    {
        load_data_file(elem);
    }
}

void Resources::load_pack_data(Asset* asset, Resources::PackType type)
{
    std::string prefix = "";
    switch (type) {
    case PackType::Texture_Type:
        prefix = "t";
        break;
    case PackType::Model_Type:
        prefix = "m";
        break;
    case PackType::Shader_Type:
        prefix = "s";
        break;
    case PackType::Data_File_Type:
        prefix = "d";
        break;
    }

    std::string data_dir = Get_Data_Director();
    std::string file_name = prefix + pad_int(asset->pack_index, 3) + ".pack";
    std::string file_path = data_dir + file_name;

    asset->data = new char[asset->data_size];
    Utilities::Read_File_Bytes(file_path, asset->pack_offset, asset->data_size, (char*)asset->data);
    
    unsigned char* compressed_data = (unsigned char*)asset->data;
    std::vector<unsigned char> decompressed_data = Utilities::Decompress(std::vector<unsigned char>(compressed_data, compressed_data + asset->data_size));

    delete[] asset->data;

    asset->data = new char[decompressed_data.size()];
    memcpy(asset->data, decompressed_data.data(), decompressed_data.size());
    asset->data_size = decompressed_data.size();
}

Texture* Resources::get_texture(std::string name)
{
    if (!Has_Texture(name)) {
        Logger::LogError(LOG_POS("get_texture"), "Texture not found: %s \n", name.c_str());
        return nullptr;
    }

    Load_Texture(name);

    return static_cast<Texture*>(m_texture_assets[name].handle);
}

std::string Resources::get_shader_file(std::string name)
{
    if (!Has_Shader(name)) {
        Logger::LogError(LOG_POS("get_shader_file"), "Shader File not found: %s \n", name.c_str());
        return std::string();
    }

    Load_Shader(name);

    return m_shader_assets[name].path;
}

std::vector<char> Resources::get_shader_bin(std::string name)
{
    if (!Has_Shader(name)) {
        Logger::LogError(LOG_POS("get_shader_bin"), "Shader File not found: %s \n", name.c_str());
        return std::vector<char>();
    }

    Load_Shader(name);

    char* data_ptr = (char*)m_shader_assets[name].data;
    size_t data_size = m_shader_assets[name].data_size;

    return std::vector<char>(data_ptr, data_ptr + data_size);
}

void Resources::modify_shader_bin(std::string name, std::vector<char> data)
{
    if (!Has_Shader(name)) {
        Logger::LogError(LOG_POS("modify_shader_bin"), "Shader File not found: %s \n", name.c_str());
        return;
    }

    Load_Shader(name);

    delete[] m_shader_assets[name].data;
    m_shader_assets[name].data = new char[data.size()];
    m_shader_assets[name].data_size = data.size();
    memcpy(m_shader_assets[name].data, data.data(), data.size());
}

Model* Resources::get_model(std::string name)
{
    if (!Has_Model(name)) {
        Logger::LogError(LOG_POS("get_model"), "Model not found: %s \n", name.c_str());
        return nullptr;
    }

    Load_Model(name);

    return static_cast<Model*>(m_models_assets[name].handle);
}

std::string Resources::get_data_file_string(std::string name)
{
    if (!Has_Data_File(name)) {
        Logger::LogError(LOG_POS("get_data_file_string"), "Data File not found: %s \n", name.c_str());
        return "";
    }

    Load_Data_File(name);

    std::string res;
    res.assign((char*)m_data_assets[name].data, m_data_assets[name].data_size);
    return res;
}

std::vector<char> Resources::get_data_file_bin(std::string name)
{
    if (!Has_Model(name)) {
        Logger::LogError(LOG_POS("get_data_file_string"), "Data File not found: %s \n", name.c_str());
        return std::vector<char>();
    }

    Load_Data_File(name);

    char* dta_ptr = (char*)m_data_assets[name].data;
    size_t dta_size = m_data_assets[name].data_size;
    return std::vector<char>(dta_ptr, dta_ptr + dta_size);
}

std::string Resources::Get_Resources_Director()
{
#ifdef NDEBUG
	// release
	return Utilities::Get_Root_Directory() + "resources\\";
#else
	//debug
	return (Utilities::Get_Root_Directory() + "..\\..\\..\\resources\\");
#endif

}

std::string Resources::Get_Data_Director()
{
    return Utilities::Get_Root_Directory() + "data\\";
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

void Resources::get_binary_assets(std::string data_Path, std::vector<Asset>& assets)
{
    std::string base64 = Utilities::Read_File_String(data_Path);
    if (base64 == "") {
        Logger::LogFatal(LOG_POS("get_binary_assets"), "No shader assets to available.");
        return;
    }
    std::string resource_map_str = Utilities::Decode_Base64(base64);

    auto data = json::parse(resource_map_str);
    for (auto& element : data) {

        //std::cout << element << '\n';
        Asset asset;

        element["resource_name"].get_to(asset.name);
        element["pack_file"].get_to(asset.pack_index);
        element["size"].get_to(asset.data_size);
        element["offset"].get_to(asset.pack_offset);

        if (element.contains("use_spirv")) {
            element["use_spirv"].get_to(asset.use_spirv);
        }
        else {
            asset.use_spirv = false;
        }


        asset.path = "";
        asset.loaded = false;
        asset.handle = nullptr;
        asset.data = nullptr;

        assets.push_back(asset);
    }

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
    std::string data_dir = Get_Data_Director();
    std::string shader_data_file = data_dir + "s000.dat";
    std::string shader_pack_file = data_dir + "s001.pack";

    std::vector<Asset> assets;

    get_binary_assets(shader_data_file, assets);

    std::vector<char> pack_data = Utilities::Read_File_Bytes(shader_pack_file);
    const char* data = pack_data.data();

    Logger::LogDebug(LOG_POS("load_shaders_binary"), "Loaded Shaders (%i):", (int)assets.size());
    for (auto& a : assets)
    {
        unsigned char* compressed_data = (unsigned char*)&data[a.pack_offset];
        int compressed_size = a.data_size;

        std::vector<unsigned char> decompressed_data = Utilities::Decompress(std::vector<unsigned char>(compressed_data, compressed_data + a.data_size));
        int decompressed_size = decompressed_data.size();

        delete[] a.data;
        a.data = new char[decompressed_data.size()];
        memcpy(a.data, decompressed_data.data(), decompressed_data.size());

        a.data_size = decompressed_data.size();
        a.loaded = true;
        m_shader_assets[a.name] = a;
        Logger::LogDebug(LOG_POS("load_shaders_binary"), "\t - %s (decompressed: %i bytes, compressed: %i bytes)", a.name.c_str(), decompressed_size, compressed_size);
    }
}

void Resources::load_textures_binary()
{
    std::string data_dir = Get_Data_Director();
    std::string texture_data_file = data_dir + "t000.dat";

    std::vector<Asset> assets;

    get_binary_assets(texture_data_file, assets);

    Logger::LogDebug(LOG_POS("load_textures_binary"), "Loaded Textures:");
    for (auto& a : assets)
    {
        m_texture_assets[a.name] = a;
        Logger::LogDebug(LOG_POS("load_textures_binary"), "\t - %s", a.name.c_str());
    }
}

void Resources::load_models_binary()
{
    std::string data_dir = Get_Data_Director();
    std::string model_data_file = data_dir + "m000.dat";

    std::vector<Asset> assets;

    get_binary_assets(model_data_file, assets);

    Logger::LogDebug(LOG_POS("load_models_binary"), "Loaded Models:");
    for (auto& a : assets)
    {
        m_models_assets[a.name] = a;
        Logger::LogDebug(LOG_POS("load_models_binary"), "\t - %s", a.name.c_str());
    }
}

void Resources::load_data_binary()
{
    std::string data_dir = Get_Data_Director();
    std::string data_data_file = data_dir + "d000.dat";

    std::vector<Asset> assets;

    get_binary_assets(data_data_file, assets);

    Logger::LogDebug(LOG_POS("load_data_binary"), "Loaded Data Files:");
    for (auto& a : assets)
    {
        m_data_assets[a.name] = a;
        Logger::LogDebug(LOG_POS("load_data_binary"), "\t - %s", a.name.c_str());
    }

}


