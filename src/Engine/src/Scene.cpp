#include "Scene.h"

#include "Engine.h"
#include "WorldObject.h"
#include "Logger.h"

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void Scene::Activate(bool active)
{
	if (active && m_active)
		return;

	m_active = active;

	Engine::Activate_Scene(this);
	Initialize();
}

void Scene::Initialize()
{
	Init();
}

void Scene::Load_Json_String(std::string json_str)
{
}

void Scene::Load_Json(json data)
{
	json objects = data["objects"];
}

void Scene::Load_File(std::string file)
{

}

WorldObject* Scene::Instantiate()
{
	WorldObject* obj = new WorldObject();
	m_objects.push_back(obj);
	return obj;
}

WorldObject* Scene::Instantiate(std::string name)
{
	WorldObject* obj = new WorldObject(name);
	m_objects.push_back(obj);
	return obj;
}

void Scene::Update_internal(float dt)
{
	for (const auto& obj : m_objects)
	{
		obj->DoUpdate(dt);
	}

	Update(dt);
}

void Scene::load_objects(json objects)
{
	for (auto& el : objects.items())
	{

	}

}
