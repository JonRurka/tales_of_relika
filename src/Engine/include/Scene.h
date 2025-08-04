#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

#include "Light.h"

using json = nlohmann::json;

class Engine;
class WorldObject;
class Light;

class Scene
{
	friend class Engine;
	friend class WorldObject;
public:

	struct SceneStartData {
	public:

		void setFloat(std::string name, float val) {
			m_float_settings[name] = val;
		}

		void setInt(std::string name, int val) {
			m_int_settings[name] = val;
		}

		void setBool(std::string name, bool val) {
			m_bool_settings[name] = val;
		}

		void setString(std::string name, std::string val) {
			m_string_settings[name] = val;
		}


		float getFloat(std::string name) {
			if (!m_float_settings.contains(name))
				return 0;
			return m_float_settings[name];
		}

		int getInt(std::string name) {
			if (!m_int_settings.contains(name))
				return 0;
			return m_int_settings[name];
		}

		bool getBool(std::string name) {
			if (!m_bool_settings.contains(name))
				return false;
			return m_bool_settings[name];
		}

		std::string getString(std::string name) {
			if (!m_string_settings.contains(name))
				return "";
			return m_string_settings[name];
		}

	private:
		std::map<std::string, float> m_float_settings;
		std::map<std::string, int> m_int_settings;
		std::map<std::string, std::string> m_string_settings;
		std::map<std::string, bool> m_bool_settings;
	};

	void Activate(bool active);
	void Activate(bool active, SceneStartData data);

	void Initialize();

	std::string Name() { return m_name; }

	static void Load_Json_String(std::string json_str);

	static void Load_Json(json data);

	static void Load_File(std::string file);

	WorldObject* Instantiate();

	WorldObject* Instantiate(std::string name);

	std::vector<WorldObject*> Objects() { return m_objects; }

	void Create_Ambient_Lights();

protected:
	virtual void Init() {};
	virtual void Update(float dt) {};
	virtual void Deactivate() {};

	SceneStartData Start_Data() const { return m_start_data; }

private:
	std::vector<WorldObject*> m_objects;
	std::vector<WorldObject*> m_ambient_light_objects;
	bool m_active{ false };
	std::string m_name;

	SceneStartData m_start_data{};

	void deactivate();

	void Update_internal(float dt);

	void remove_object_from_scene(WorldObject* obj);

	void load_objects(json objects);

	void create_light_object(WorldObject** obj, Light** light_comp, Light::Light_Type type, glm::vec3 pos, float scale, glm::vec4 color);
	void create_ambient_light(glm::fvec3 dir);

	inline static const std::string LOG_LOC{ "SCENE" };
};