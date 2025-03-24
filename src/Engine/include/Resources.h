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
	
	Resources();
	
	Resources(Resources::LoadMode mode);

	static void Load_Shader(std::string name) { m_instance->load_shader(name); }
	static void Load_Shader(std::vector<std::string> names) { m_instance->load_shader(names); }

	static void Load_Texture(std::string name) { m_instance->load_texture(name); }
	static void Load_Texture(std::vector<std::string> names){ m_instance->load_texture(names);}

	static void Load_Model(std::string name) { m_instance->load_model(name); }
	static void Load_Model(std::vector<std::string> names){ m_instance->load_model(names);}

	static Texture* Get_Texture(std::string name) { return m_instance->get_texture(name); }
	static std::string Get_Shader_File(std::string name){ return m_instance->get_shader_file(name);}
	static Model* Get_Model(std::string name) { return m_instance->get_model(name); }
	
	static bool Has_Texture(std::string name) { return m_instance->has_texture(name); }

	static bool Has_Shader(std::string name) { return m_instance->has_shader(name); }

	static bool Has_Model(std::string name) { return m_instance->has_model(name); }

	static std::string Get_Resources_Director();

private:

	struct Asset {
	public:
		std::string name;
		std::string path;
		bool loaded;
		void* handle;
	};

	LoadMode m_mode;

	std::unordered_map<std::string, Asset> m_shader_assets;
	std::unordered_map<std::string, Asset> m_texture_assets;
	std::unordered_map<std::string, Asset> m_models_assets;

	static Resources* m_instance;

	static void get_assets_recursively(std::string basePath, std::vector<Asset>& assets, std::string rel_path = "");

	void load_shaders_binary();
	void load_textures_binary();
	void load_models_binary();

	void load_shaders_fs();
	void load_textures_fs();
	void load_models_fs();


	void load_shader(std::string name);
	void load_shader(std::vector<std::string> names);

	void load_texture(std::string name);
	void load_texture(std::vector<std::string> names);

	void load_model(std::string name);
	void load_model(std::vector<std::string> names);

	Texture* get_texture(std::string name);
	std::string get_shader_file(std::string name);
	Model* get_model(std::string name);

	bool has_texture(std::string name) {
		return m_texture_assets.find(name) != m_texture_assets.end();
	}

	bool has_shader(std::string name) {
		return m_shader_assets.find(name) != m_shader_assets.end();
	}

	bool has_model(std::string name) {
		return m_models_assets.find(name) != m_models_assets.end();
	}

	const std::string LOG_LOC{ "RESOURCES" };
};