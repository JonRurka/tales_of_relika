#pragma once

#include <string>
#include <vector>
#include <type_traits>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include "Component.h"

class Transform;
class MeshRenderer;
class Material;
class Model;

class WorldObject
{
public:

	WorldObject();

	WorldObject(std::string name);

	Transform* Get_Transform() { return m_transform; }
	MeshRenderer* Get_MeshRenderer() { return m_renderer; }

	void Parent(WorldObject* value);
	WorldObject* Parent() { return m_parent; }

	void Name(std::string value) { m_name = value; }
	std::string Name() { return m_name; }

	std::vector<WorldObject*> Children() { return m_children; }

	void Translate(float x, float y, float z);

	void Rotate(float x, float y, float z);

	void Scale(float x, float y, float z);

	template<typename T, 
		typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
	T* Add_Component() 
	{
		T* comp = new T();
		Initialize_Component(static_cast<Component*>(comp));
		return comp;
	}

	// TODO: move to private and call from friend controller class
	void DoUpdate(float dt);

	static WorldObject* Instantiate(Model* model, Material* mat, WorldObject* parent = nullptr);

	static WorldObject* Load(json data);

	//static void Update_Objects(float dt);

private:
	std::string m_name;
	Transform* m_transform{nullptr};
	MeshRenderer* m_renderer{ nullptr };
	WorldObject* m_parent{ nullptr };
	
	static std::vector<WorldObject*> m_all_objects;

	std::vector<Component*> m_components;
	std::vector<WorldObject*> m_children;

	void Initialize_Component(Component* comp);

	static void Add_Object(WorldObject* object);

	inline static const std::string LOG_LOC{ "WORLD_OBJECT" };
};

