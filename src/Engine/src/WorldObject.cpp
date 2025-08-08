#include "WorldObject.h"

#include "Engine.h"
#include "Scene.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Component.h"
#include "Model.h"
#include "Mesh.h"
#include "Logger.h"

#define DEFAULT_NAME "new_worldobject"

int WorldObject::m_next_idx{ 0 };

WorldObject::WorldObject(int id, std::weak_ptr<Scene> scene)
{
	m_name = DEFAULT_NAME;
	m_scene = scene;
	m_transform = std::make_shared<Transform>(shared_from_this());
	m_renderer = std::make_shared<MeshRenderer>(shared_from_this());
	m_object_idx = id;
	m_enabled = true;
}

WorldObject::WorldObject(int id, std::weak_ptr<Scene> scene, std::string name)
{
	m_name = name;
	m_scene = scene;
	m_transform = std::make_shared<Transform>(shared_from_this());
	m_renderer = std::make_shared<MeshRenderer>(shared_from_this());
	m_object_idx = id;
	m_enabled = true;
}

WorldObject::~WorldObject()
{
	Destroy();
}

Transform& WorldObject::Get_Transform()
{
	return *m_transform.get();
}

MeshRenderer& WorldObject::Get_MeshRenderer()
{
	return *m_renderer.get();
}

void WorldObject::Parent(std::weak_ptr<WorldObject> value)
{
	assert(!value.expired());
	m_parent = value;
	m_parent.lock()->m_children.push_back(shared_from_this());
}

Scene& WorldObject::scene()
{
	assert(!m_scene.expired());
	return *m_scene.lock();
}

void WorldObject::Translate(float x, float y, float z)
{
	m_transform->Translate(x, y, z);
}

void WorldObject::Rotate(float x, float y, float z)
{
	m_transform->Rotate(x, y, z);
}

void WorldObject::Scale(float x, float y, float z)
{
	m_transform->Scale(x, y, z);
}

void WorldObject::DoUpdate(float dt)
{
	//Logger::LogDebug(LOG_POS("DoUpdate"), "%s updated.", Name().c_str());

	if (!Enabled()) {
		return;
	}

	m_transform->Update(dt);
	m_renderer->Update(dt);
	for (const auto& pair : m_components) {
		pair.second->Update(dt);
	}
}

void WorldObject::Destroy()
{
	Enabled(false);

	for (const auto& pair : m_components) {
		pair.second->Destroy();
	}
	m_components.clear();

	//m_transform.Destroy();
	//m_renderer.Destroy();

	//Remove_Object(m_object_idx);
	scene().remove_object_from_scene(m_object_idx);
}

void WorldObject::Initialize_Component(std::shared_ptr<Component> comp)
{
	m_next_comp_idx++;
	comp->Object(shared_from_this());
	m_components[m_next_comp_idx] = comp;
	comp->Component_Index(m_next_comp_idx);
	comp->Init();
}

void WorldObject::Remove_Component(int comp_idx)
{
	if (!m_components.contains(comp_idx))
		return;
	m_components[comp_idx]->Destroy();
	m_components.erase(comp_idx);
}

std::weak_ptr<WorldObject> WorldObject::Instantiate(std::shared_ptr<Model> model, std::shared_ptr<Material> mat, std::shared_ptr<WorldObject> parent)
{
	//WorldObject* obj = new WorldObject(Engine::Active_Scene_Ptr(), model->Name());
	std::shared_ptr<WorldObject> obj = Engine::Active_Scene().Instantiate(model->Name()).lock();
	if (parent != nullptr) {
		obj->Parent(parent);
		obj->Get_Transform().set_position(model->mesh()[0]->Center());
	}

	if (model->mesh().size() > 0) {
		obj->Get_MeshRenderer().Set_Material(mat);
		obj->Get_MeshRenderer().Set_Mesh(model->mesh()[0]); // TODO: Change this.
	}

	for (int i = 0; i < model->Children().size(); i++)
	{
		obj->m_children.push_back(WorldObject::Instantiate(model->Children()[i], mat, obj));
	}

	return obj;
}

WorldObject* WorldObject::Load(json data)
{
	/*
	std::string name;
	data["name"].get_to(name);
	WorldObject* obj = new WorldObject(Engine::Active_Scene(), name);

	json components = data["components"];
	for (auto& el : components.items())
	{
		json comp_data = el.value();
		
	}

	return obj;*/
	return nullptr;
}

/*void WorldObject::Update_Objects(float dt) {
	for (const auto& obj : m_all_objects)
	{
		obj->DoUpdate(dt);
	}
}*/

int WorldObject::Add_Object(WorldObject* object)
{
	//printf("Add world object: %s\n", object->Name().c_str());
	//m_all_objects.push_back(object);
	//int cur_idx = m_next_idx;
	//m_all_objects[cur_idx] = object;
	//m_next_idx++;
	return m_next_idx++;
}

void WorldObject::Remove_Object(int idx)
{
	/*if (!m_all_objects.contains(idx)) {
		return;
	}
	WorldObject* obj = m_all_objects[idx];
	obj->scene()->remove_object_from_scene(obj);
	delete m_all_objects[idx];
	m_all_objects.erase(idx);*/
}
