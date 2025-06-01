#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class Texture;
class Shader;
class Model;

class Resources {
public:

	enum class LoadMode {
		Binary,
		Filesystem
	};

	struct Asset {
	public:
		std::string name;
		std::string path;
		bool loaded;
		void* handle;
		void* data;
		size_t data_size;
		int pack_index;
		int pack_offset;

		// shader specific
		bool use_spirv;
	};
	
	Resources();
	
	Resources(Resources::LoadMode mode);

	static void Load_Shader(std::string name) { m_instance->load_shader(name); }
	static void Load_Shader(std::vector<std::string> names) { m_instance->load_shader(names); }

	static void Load_Texture(std::string name, bool flip = true) { m_instance->load_texture(name, flip); }
	static void Load_Texture(std::vector<std::string> names, bool flip = true){ m_instance->load_texture(names, flip);}

	static void Load_Model(std::string name) { m_instance->load_model(name); }
	static void Load_Model(std::vector<std::string> names){ m_instance->load_model(names);}

	static void Load_Data_File(std::string name) { m_instance->load_data_file(name); }
	static void Load_Data_File(std::vector<std::string> names) { m_instance->load_data_file(names); }

	static Texture* Get_Texture(std::string name) { return m_instance->get_texture(name); }
	static std::string Get_Shader_File(std::string name){ return m_instance->get_shader_file(name);}
	static std::vector<char> Get_Shader_bin(std::string name) { return m_instance->get_shader_bin(name); }
	static void Modify_Shader_Bin(std::string name, std::vector<char> data) { return m_instance->modify_shader_bin(name, data); }
	static Model* Get_Model(std::string name) { return m_instance->get_model(name); }

	static std::string Get_Data_File_String(std::string name) { return m_instance->get_data_file_string(name); }
	static std::vector<char> Get_Data_File_Bin(std::string name) { return m_instance->get_data_file_bin(name); }


	static Asset Get_Texture_Asset(std::string name) {
		if (!Has_Texture(name))
			return Asset();
		Load_Texture(name);
		return m_instance->m_texture_assets[name];
	}

	static Asset Get_Shader_Asset(std::string name) {
		if (!Has_Shader(name))
			return Asset();
		Load_Shader(name);
		return m_instance->m_shader_assets[name];
	}

	static Asset Get_Model_Asset(std::string name) {
		if (!Has_Model(name))
			return Asset();
		Load_Model(name);
		return m_instance->m_models_assets[name];
	}

	static Asset Get_Data_Asset(std::string name) {
		if (!Has_Model(name))
			return Asset();
		Load_Data_File(name);
		return m_instance->m_data_assets[name];
	}
	
	static bool Has_Texture(std::string name) { return m_instance->has_texture(name); }

	static bool Has_Shader(std::string name) { return m_instance->has_shader(name); }

	static bool Has_Model(std::string name) { return m_instance->has_model(name); }

	static bool Has_Data_File(std::string name) { return m_instance->has_data_file(name); }

	static std::string Get_Resources_Director();

	static std::string Get_Data_Director();

private:

	LoadMode m_mode;

	enum PackType {
		Shader_Type,
		Texture_Type,
		Model_Type,
		Data_File_Type
	};

	std::unordered_map<std::string, Asset> m_shader_assets;
	std::unordered_map<std::string, Asset> m_texture_assets;
	std::unordered_map<std::string, Asset> m_models_assets;
	std::unordered_map<std::string, Asset> m_data_assets;

	static Resources* m_instance;

	static void get_assets_recursively(std::string basePath, std::vector<Asset>& assets, std::string rel_path = "");

	static void get_binary_assets(std::string data_Path, std::vector<Asset>& assets);

	void load_shaders_binary();
	void load_textures_binary();
	void load_models_binary();
	void load_data_binary();

	void load_shaders_fs();
	void load_textures_fs();
	void load_models_fs();
	void load_data_fs(){}


	void load_shader(std::string name);
	void load_shader(std::vector<std::string> names);

	void load_texture(std::string name, bool flip = true);
	void load_texture(std::vector<std::string> names, bool flip = true);

	void load_model(std::string name);
	void load_model(std::vector<std::string> names);

	void load_data_file(std::string name);
	void load_data_file(std::vector<std::string> names);

	void load_pack_data(Asset* asset, Resources::PackType type);

	Texture* get_texture(std::string name);

	std::string get_shader_file(std::string name);
	std::vector<char> get_shader_bin(std::string name);
	void modify_shader_bin(std::string name, std::vector<char> data);
	Model* get_model(std::string name);

	std::string get_data_file_string(std::string name);
	std::vector<char> get_data_file_bin(std::string name);

	bool has_texture(std::string name) {
		return m_texture_assets.find(name) != m_texture_assets.end();
	}

	bool has_shader(std::string name) {
		return m_shader_assets.find(name) != m_shader_assets.end();
	}

	bool has_model(std::string name) {
		return m_models_assets.find(name) != m_models_assets.end();
	}

	bool has_data_file(std::string name) {
		return m_data_assets.find(name) != m_data_assets.end();
	}

	inline static const std::string LOG_LOC{ "RESOURCES" };
};