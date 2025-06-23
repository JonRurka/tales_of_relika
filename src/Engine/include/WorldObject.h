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
class Scene;

class WorldObject
{
	friend class Component;
public:

	WorldObject(Scene* scene);

	WorldObject(Scene* scene,std::string name);

	Transform* Get_Transform() { return m_transform; }
	MeshRenderer* Get_MeshRenderer() { return m_renderer; }

	void Parent(WorldObject* value);
	WorldObject* Parent() { return m_parent; }

	Scene* scene() { return m_scene; }

	void Name(std::string value) { m_name = value; }
	std::string Name() { return m_name; }

	void Enabled(bool val) { m_enabled = val; }
	bool Enabled() { return m_enabled; }

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

	void Destroy();

	static WorldObject* Instantiate(Model* model, Material* mat, WorldObject* parent = nullptr);

	static WorldObject* Load(json data);

	//static void Update_Objects(float dt);

private:
	std::string m_name;
	Scene* m_scene{ nullptr };
	Transform* m_transform{nullptr};
	MeshRenderer* m_renderer{ nullptr };
	WorldObject* m_parent{ nullptr };
	bool m_enabled{ false };
	int m_object_idx{ 0 };
	
	//static std::vector<WorldObject*> m_all_objects;
	static std::unordered_map<int, WorldObject*> m_all_objects;
	static int m_next_idx;

	std::vector<Component*> m_components;
	std::vector<WorldObject*> m_children;

	void Initialize_Component(Component* comp);

	void Remove_Component(int comp_idx);

	static int Add_Object(WorldObject* object);

	static void Remove_Object(int idx);

	inline static const std::string LOG_LOC{ "WORLD_OBJECT" };
};

