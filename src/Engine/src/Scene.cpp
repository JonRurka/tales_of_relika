#include "Scene.h"

#include "Engine.h"
#include "WorldObject.h"
#include "Logger.h"
#include "Light.h"
#include "Transform.h"


#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void Scene::Activate(bool active)
{
	Activate(active, SceneStartData{});
}

void Scene::Activate(bool active, SceneStartData data)
{
	if (active && m_active)
		return;
	if (!active && !m_active)
		return;

	m_active = active;

	Engine::Activate_Scene(Name());

	if (m_active) 
	{
		m_start_data = data;
		Initialize();
	}
	else
	{
		deactivate();
	}
}

void Scene::Initialize()
{
	Logger::LogInfo(LOG_POS("Initialize"), "Initializing scene %s.", m_name.c_str());
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

std::weak_ptr<WorldObject> Scene::Instantiate()
{
	m_next_idx++;
	std::shared_ptr<WorldObject> obj = std::make_shared<WorldObject>(m_next_idx, Engine::Get_Scene_Ptr(m_name));
	obj->scene_init();
	m_objects[m_next_idx] = obj;
	return obj;
}

std::weak_ptr<WorldObject> Scene::Instantiate(std::string name)
{
	m_next_idx++;
	//Logger::LogDebug(LOG_POS("Instantiate"), "Shared Count: %i", Engine::Get_Scene_Ptr(m_name).use_count());
	// TODO: fix using shared_from_This()
	std::shared_ptr<WorldObject> obj = std::make_shared<WorldObject>(m_next_idx, Engine::Get_Scene_Ptr(m_name), name);
	obj->scene_init();
	m_objects[m_next_idx] = obj;
	return obj;
}

std::vector<std::shared_ptr<WorldObject>> Scene::Objects()
{
	std::vector<std::shared_ptr<WorldObject>> res;
	res.reserve(m_objects.size());
	for (const auto& pair : m_objects)
	{
		res.push_back(pair.second);
	}
	return res;
}

void Scene::Create_Ambient_Lights() {

	create_ambient_light(glm::fvec3(1, 0, 0));
	create_ambient_light(glm::fvec3(-1, 0, 0));

	create_ambient_light(glm::fvec3(0, 1, 0));
	create_ambient_light(glm::fvec3(0, -1, 0));

	create_ambient_light(glm::fvec3(0, 0, 1));
	create_ambient_light(glm::fvec3(0, 0, -1));
}

void Scene::create_ambient_light(glm::fvec3 dir) 
{
	std::weak_ptr<WorldObject> light_obj_dir;
	std::weak_ptr<Light> light_comp_dir;
	glm::vec4 light_color_dir = glm::fvec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 light_pos_dir = glm::vec3(0.0f, 0.0f, 100.0f);
	create_light_object(&light_obj_dir, &light_comp_dir, Light::Light_Type::DIRECTIONAL, light_pos_dir, 1, light_color_dir);
	light_comp_dir.lock()->Enabled(true);
	light_obj_dir.lock()->Get_Transform().LookAt(light_pos_dir + dir);
	light_comp_dir.lock()->Strength(0.2f);
	m_ambient_light_objects.push_back(light_obj_dir);
}

void Scene::create_light_object(std::weak_ptr<WorldObject>* obj, std::weak_ptr<Light>* light_comp, Light::Light_Type type, glm::vec3 pos, float scale, glm::vec4 color)
{
	*obj = Instantiate("light"); //new WorldObject("light");
	std::weak_ptr<WorldObject> w_obj = *obj;
	//light_obj->Get_MeshRenderer()->Set_Shader(m_light_shader); // m_light_shader
	//w_obj->Get_MeshRenderer()->Set_Material(light_material);
	//w_obj->Get_MeshRenderer()->Set_Mesh(light_mesh);
	//((Light_Material*)w_obj->Get_MeshRenderer()->Get_Material())->Light_Color(color);
	w_obj.lock()->Get_Transform().Translate(pos);
	w_obj.lock()->Get_Transform().Scale(glm::vec3(scale, scale, scale));

	*light_comp = w_obj.lock()->Add_Component<Light>();
	std::weak_ptr<Light> l_comp = *light_comp;
	l_comp.lock()->Type(type);
	l_comp.lock()->Color(color);
	l_comp.lock()->Strength(2.0f);
	l_comp.lock()->Linear_Coefficient(0.027f);
	l_comp.lock()->Quadratic_Coefficient(0.0028f);
	l_comp.lock()->CutOff(glm::cos(glm::radians(12.5f)));
	l_comp.lock()->OuterCutOff(glm::cos(glm::radians(15.5f)));
}


void Scene::deactivate()
{
	Logger::LogInfo(LOG_POS("deactivate"), "Deactivating scene %s.", m_name.c_str());
	Deactivate();

	m_objects.clear();
	m_ambient_light_objects.clear();
}

void Scene::Update_internal(float dt)
{
	for (const auto& pair : m_objects)
	{
		pair.second->DoUpdate(dt);
	}

	Update(dt);
}

void Scene::remove_object_from_scene(int id)
{
	if (m_objects.contains(id))
	{
		m_objects.erase(id);
	}
	//Remove_If_Found(m_objects, id);
}

void Scene::load_objects(json objects)
{
	for (auto& el : objects.items())
	{

	}

}
