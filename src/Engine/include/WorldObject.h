#pragma once

#include <string>
#include <vector>
#include <type_traits>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include "Component.h"
#include "Transform.h"
#include "MeshRenderer.h"

class Transform;
class MeshRenderer;
class Material;
class Model;
class Scene;

class WorldObject : public std::enable_shared_from_this<WorldObject>
{
	friend class Component;
public:

	WorldObject(int id, std::weak_ptr<Scene> scene);

	WorldObject(int id, std::weak_ptr<Scene> scene, std::string name);

	~WorldObject();

	Transform& Get_Transform() { return m_transform; }
	MeshRenderer& Get_MeshRenderer() { return m_renderer; }

	void Parent(std::weak_ptr<WorldObject> value);
	bool Has_Parent() { return !m_parent.expired(); }
	std::weak_ptr<WorldObject> Parent() { return m_parent; }

	Scene& scene();

	void Name(std::string value) { m_name = value; }
	std::string Name() { return m_name; }

	void Enabled(bool val) { m_enabled = val; }
	bool Enabled() { return m_enabled; }

	std::vector<std::weak_ptr<WorldObject>> Children() { return m_children; }

	void Translate(float x, float y, float z);

	void Rotate(float x, float y, float z);

	void Scale(float x, float y, float z);

	template<typename T, 
		typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
	std::weak_ptr<T> Add_Component() 
	{
		std::shared_ptr<T> comp = std::make_shared<T>();
		Initialize_Component(std::static_pointer_cast<Component>(comp));
		return comp;
	}

	// TODO: move to private and call from friend controller class
	void DoUpdate(float dt);

	void Destroy();

	static std::weak_ptr<WorldObject> Instantiate(Model* model, Material* mat, std::shared_ptr<WorldObject> parent = nullptr);

	static WorldObject* Load(json data);

	//static void Update_Objects(float dt);

private:
	std::string m_name;
	std::weak_ptr<Scene> m_scene;
	Transform m_transform;
	MeshRenderer m_renderer;
	std::weak_ptr<WorldObject> m_parent;
	bool m_enabled{ false };
	int m_object_idx{ 0 };
	
	static int m_next_idx;

	std::vector<std::shared_ptr<Component>> m_components;
	std::vector<std::weak_ptr<WorldObject>> m_children;

	void Initialize_Component(std::shared_ptr<Component> comp);

	void Remove_Component(int comp_idx);

	static int Add_Object(WorldObject* object);

	static void Remove_Object(int idx);

	inline static const std::string LOG_LOC{ "WORLD_OBJECT" };
};

