#include "WorldObject.h"

#include "Engine.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Component.h"
#include "Model.h"
#include "Mesh.h"
#include "Logger.h"

#define DEFAULT_NAME "new_worldobject"

std::vector<WorldObject*> WorldObject::m_all_objects;

WorldObject::WorldObject(Scene* scene)
{
	m_name = DEFAULT_NAME;
	m_scene = scene;
	m_transform = new Transform(this);
	m_renderer = new MeshRenderer(this);
	Add_Object(this);
}

WorldObject::WorldObject(Scene* scene, std::string name)
{
	m_name = name;
	m_scene = scene;
	m_transform = new Transform(this);
	m_renderer = new MeshRenderer(this);
	Add_Object(this);
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
	m_transform->Update(dt);
	m_renderer->Update(dt);
	for (const auto& comp : m_components) {
		comp->Update(dt);
	}
}

void WorldObject::Initialize_Component(Component* comp)
{
	comp->Object(this);
	m_components.push_back(comp);
	comp->Init();
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

/*void WorldObject::Update_Objects(float dt)
{
	for (const auto& obj : m_all_objects)
	{
		obj->DoUpdate(dt);
	}
}*/

void WorldObject::Add_Object(WorldObject* object)
{
	//printf("Add world object: %s\n", object->Name().c_str());
	m_all_objects.push_back(object);
}
