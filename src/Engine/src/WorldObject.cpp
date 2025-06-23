#include "WorldObject.h"

#include "Engine.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Component.h"
#include "Model.h"
#include "Mesh.h"
#include "Logger.h"

#define DEFAULT_NAME "new_worldobject"

std::unordered_map<int, WorldObject*> WorldObject::m_all_objects;
int WorldObject::m_next_idx{ 0 };

WorldObject::WorldObject(Scene* scene)
{
	m_name = DEFAULT_NAME;
	m_scene = scene;
	m_transform = new Transform(this);
	m_renderer = new MeshRenderer(this);
	m_object_idx = Add_Object(this);
	m_enabled = true;
}

WorldObject::WorldObject(Scene* scene, std::string name)
{
	m_name = name;
	m_scene = scene;
	m_transform = new Transform(this);
	m_renderer = new MeshRenderer(this);
	m_object_idx = Add_Object(this);
	m_enabled = true;
}

void WorldObject::Parent(WorldObject* value)
{
	m_parent = value;
	m_parent->m_children.push_back(this);
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
	for (const auto& comp : m_components) {
		comp->Update(dt);
	}
}

void WorldObject::Destroy()
{
	Enabled(false);

	for (const auto& comp : m_components) {
		comp->Destroy();
	}
	m_components.clear();

	m_transform->Destroy();
	delete m_transform;

	m_renderer->Destroy();
	delete m_renderer;

	Remove_Object(m_object_idx);
}

void WorldObject::Initialize_Component(Component* comp)
{
	comp->Object(this);
	m_components.push_back(comp);
	comp->Component_Index(m_components.size() - 1);
	comp->Init();
}

void WorldObject::Remove_Component(int comp_idx)
{
	// TODO: This should probably be a map instead of an ever-growing list.
	Component* comp = m_components[comp_idx];
	delete comp;
}

WorldObject* WorldObject::Instantiate(Model* model, Material* mat, WorldObject* parent)
{
	WorldObject* obj = new WorldObject(Engine::Active_Scene(), model->Name());
	if (parent != nullptr) {
		obj->Parent(parent);
		obj->Get_Transform()->set_position(model->mesh()[0]->Center());
	}

	if (model->mesh().size() > 0) {
		obj->Get_MeshRenderer()->Set_Material(mat);
		obj->Get_MeshRenderer()->Set_Mesh(model->mesh()[0]);
	}

	for (int i = 0; i < model->Children().size(); i++)
	{
		obj->m_children.push_back(WorldObject::Instantiate(model->Children()[i], mat, obj));
	}

	return obj;
}

WorldObject* WorldObject::Load(json data)
{
	std::string name;
	data["name"].get_to(name);
	WorldObject* obj = new WorldObject(Engine::Active_Scene(), name);

	json components = data["components"];
	for (auto& el : components.items())
	{
		json comp_data = el.value();
		
	}

	return obj;
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
	int cur_idx = m_next_idx;
	m_all_objects[cur_idx] = object;
	m_next_idx++;
	return cur_idx;
}

void WorldObject::Remove_Object(int idx)
{
	if (!m_all_objects.contains(idx)) {
		return;
	}
	delete m_all_objects[idx];
	m_all_objects.erase(idx);
}
