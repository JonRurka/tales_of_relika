#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class WorldObject;

class Scene
{
	friend class Engine;
public:

	void Activate(bool active);

	void Initialize();

	static void Load_Json_String(std::string json_str);

	static void Load_Json(json data);

	static void Load_File(std::string file);

	WorldObject* Instantiate();

	WorldObject* Instantiate(std::string name);

protected:
	virtual void Init() {};
	virtual void Update(float dt) {};

private:
	std::vector<WorldObject*> m_objects;
	bool m_active{ false };

	void Update_internal(float dt);

	void load_objects(json objects);

	inline static const std::string LOG_LOC{ "SCENE" };
};