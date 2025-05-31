#include "VoxelWorld_Scene.h"

#include "Game_Resources.h"

#include "WorldGenController.h"

#include "Editor_Camera_Control.h"

void VoxelWorld_Scene::Init()
{
	setup_camera();
	setup_lights();
	setup_chunk_gen();
}

void VoxelWorld_Scene::Update(float dt)
{
	//Logger::LogDebug(LOG_POS("Update"), "update");
}



void VoxelWorld_Scene::setup_camera()
{
	// Create camera
	std::vector<std::string> faces
	{
		Game_Resources::Textures::SKYBOX_RIGHT,
		Game_Resources::Textures::SKYBOX_LEFT,
		Game_Resources::Textures::SKYBOX_TOP,
		Game_Resources::Textures::SKYBOX_BOTTOM,
		Game_Resources::Textures::SKYBOX_FRONT,
		Game_Resources::Textures::SKYBOX_BACK
	};
	Cubemap* skybox_cubmap = new Cubemap(faces, false);
	Camera_obj = Instantiate("camera");
	//Camera_obj->Get_Transform()->Position(glm::vec3(0, 5, 6));
	Camera_obj->Get_Transform()->Position(glm::vec3(0, 10, -50));
	Camera_obj->Get_Transform()->LookAt(glm::vec3(0.0f, 10.0f, 100.0f));
	camera = Camera_obj->Add_Component<Camera>();
	Editor_Camera_Control* cam_control = Camera_obj->Add_Component<Editor_Camera_Control>();
	cam_control->Speed(10.0f);
	//camera->Clear_Color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//camera->FOV(90.0f);
	camera->Set_Skybox(skybox_cubmap);

	/*
	Light* l_comp = Camera_obj->Add_Component<Light>();
	l_comp->Type(Light::Light_Type::SPOT);
	l_comp->Color(glm::vec4(1, 1, 1, 1));
	l_comp->Strength(2.0f);
	l_comp->Linear_Coefficient(0.027f);
	l_comp->Quadratic_Coefficient(0.0028f);
	l_comp->CutOff(glm::cos(glm::radians(12.5f)));
	l_comp->OuterCutOff(glm::cos(glm::radians(15.5f)));*/
}

void VoxelWorld_Scene::setup_lights()
{
	Create_Ambient_Lights();
	
	// Create Sun Directional light
	glm::vec4 light_color_dir = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 light_pos_dir = glm::vec3(0.0f, 0.0f, 100.0f);
	create_light_object(&light_obj_dir, &light_comp_dir, Light::Light_Type::DIRECTIONAL, light_pos_dir, 1, light_color_dir);
	light_comp_dir->Enabled(true);
	light_obj_dir->Get_Transform()->LookAt(glm::vec3(10.0f, -50.0f, -20.0f));
	light_comp_dir->Strength(0.9f);
}

void VoxelWorld_Scene::setup_chunk_gen()
{
	world_gen_controller_obj = Instantiate("World_Gen_Controller");
	world_gen_controller = world_gen_controller_obj->Add_Component<WorldGenController>();
}

void VoxelWorld_Scene::create_light_object(WorldObject** obj, Light** light_comp, Light::Light_Type type, glm::vec3 pos, float scale, glm::vec4 color)
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
