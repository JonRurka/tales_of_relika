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
	WorldObject* obj = new WorldObject(this);
	m_objects.push_back(obj);
	return obj;
}

WorldObject* Scene::Instantiate(std::string name)
{
	WorldObject* obj = new WorldObject(this, name);
	m_objects.push_back(obj);
	return obj;
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
	WorldObject* light_obj_dir;
	Light* light_comp_dir;
	glm::vec4 light_color_dir = glm::fvec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 light_pos_dir = glm::vec3(0.0f, 0.0f, 100.0f);
	create_light_object(&light_obj_dir, &light_comp_dir, Light::Light_Type::DIRECTIONAL, light_pos_dir, 1, light_color_dir);
	light_comp_dir->Enabled(true);
	light_obj_dir->Get_Transform()->LookAt(light_pos_dir + dir);
	light_comp_dir->Strength(0.2f);
}

void Scene::create_light_object(WorldObject** obj, Light** light_comp, Light::Light_Type type, glm::vec3 pos, float scale, glm::vec4 color)
{
	*obj = Instantiate("light"); //new WorldObject("light");
	WorldObject* w_obj = *obj;
	//light_obj->Get_MeshRenderer()->Set_Shader(m_light_shader); // m_light_shader
	//w_obj->Get_MeshRenderer()->Set_Material(light_material);
	//w_obj->Get_MeshRenderer()->Set_Mesh(light_mesh);
	//((Light_Material*)w_obj->Get_MeshRenderer()->Get_Material())->Light_Color(color);
	w_obj->Get_Transform()->Translate(pos);
	w_obj->Get_Transform()->Scale(glm::vec3(scale, scale, scale));

	*light_comp = w_obj->Add_Component<Light>();
	Light* l_comp = *light_comp;
	l_comp->Type(type);
	l_comp->Color(color);
	l_comp->Strength(2.0f);
	l_comp->Linear_Coefficient(0.027f);
	l_comp->Quadratic_Coefficient(0.0028f);
	l_comp->CutOff(glm::cos(glm::radians(12.5f)));
	l_comp->OuterCutOff(glm::cos(glm::radians(15.5f)));
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
