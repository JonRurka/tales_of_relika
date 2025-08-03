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

	void Activate(bool active);

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

private:
	std::vector<WorldObject*> m_objects;
	std::vector<WorldObject*> m_ambient_light_objects;
	bool m_active{ false };
	std::string m_name;


	void deactivate();

	void Update_internal(float dt);

	void remove_object_from_scene(WorldObject* obj);

	void load_objects(json objects);

	void create_light_object(WorldObject** obj, Light** light_comp, Light::Light_Type type, glm::vec3 pos, float scale, glm::vec4 color);
	void create_ambient_light(glm::fvec3 dir);

	inline static const std::string LOG_LOC{ "SCENE" };
};