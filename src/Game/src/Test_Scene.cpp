#include "Test_Scene.h"

#include "Standard_Material.h"
#include "Game_Resources.h"
#include "Cubemap.h"

#include <vector>
#include <string>

void Test_Scene::Init()
{
	Logger::LogInfo(LOG_POS("Init"), "Test Scene started");

	std::vector<glm::vec3> vertices = {
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),

		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),

		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),

		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),

		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),

		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f)
	};

	std::vector<glm::vec2> tex_coords = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),

		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),

		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),

		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),

		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
	};

	std::vector<glm::vec3> normals = {
		glm::vec3(0.0f,  0.0f, -1.0f),
		glm::vec3(0.0f,  0.0f, -1.0f),
		glm::vec3(0.0f,  0.0f, -1.0f),
		glm::vec3(0.0f,  0.0f, -1.0f),
		glm::vec3(0.0f,  0.0f, -1.0f),
		glm::vec3(0.0f,  0.0f, -1.0f),

		glm::vec3(0.0f,  0.0f, 1.0f),
		glm::vec3(0.0f,  0.0f, 1.0f),
		glm::vec3(0.0f,  0.0f, 1.0f),
		glm::vec3(0.0f,  0.0f, 1.0f),
		glm::vec3(0.0f,  0.0f, 1.0f),
		glm::vec3(0.0f,  0.0f, 1.0f),

		glm::vec3(-1.0f,  0.0f,  0.0f),
		glm::vec3(-1.0f,  0.0f,  0.0f),
		glm::vec3(-1.0f,  0.0f,  0.0f),
		glm::vec3(-1.0f,  0.0f,  0.0f),
		glm::vec3(-1.0f,  0.0f,  0.0f),
		glm::vec3(-1.0f,  0.0f,  0.0f),

		glm::vec3(1.0f,  0.0f,  0.0f),
		glm::vec3(1.0f,  0.0f,  0.0f),
		glm::vec3(1.0f,  0.0f,  0.0f),
		glm::vec3(1.0f,  0.0f,  0.0f),
		glm::vec3(1.0f,  0.0f,  0.0f),
		glm::vec3(1.0f,  0.0f,  0.0f),

		glm::vec3(0.0f, -1.0f,  0.0f),
		glm::vec3(0.0f, -1.0f,  0.0f),
		glm::vec3(0.0f, -1.0f,  0.0f),
		glm::vec3(0.0f, -1.0f,  0.0f),
		glm::vec3(0.0f, -1.0f,  0.0f),
		glm::vec3(0.0f, -1.0f,  0.0f),

		glm::vec3(0.0f,  1.0f,  0.0f),
		glm::vec3(0.0f,  1.0f,  0.0f),
		glm::vec3(0.0f,  1.0f,  0.0f),
		glm::vec3(0.0f,  1.0f,  0.0f),
		glm::vec3(0.0f,  1.0f,  0.0f),
		glm::vec3(0.0f,  1.0f,  0.0f),
	};

	std::vector<glm::vec3> cubePositions = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(1.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f),
	};

	glm::vec4 cube_color(1.0f, 1.0f, 1.0f, 1.0f);
	std::vector<glm::vec4> cube_colors;
	cube_colors.assign(vertices.size(), cube_color);

	Mesh* cube_mesh = new Mesh();
	cube_mesh->Vertices(vertices);
	cube_mesh->Normals(normals);
	cube_mesh->Colors(cube_colors);
	cube_mesh->TexCoords(tex_coords);


	standard_mat = new Standard_Material();
	standard_mat->SetVec3("material.ambientColor", glm::vec3(1.0f, 0.5f, 0.31f));
	standard_mat->SetVec3("material.diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
	standard_mat->SetVec2("material.scale", glm::vec2(1.0f, 1.0f));
	standard_mat->setFloat("material.shininess", 32.0f);
	standard_mat->setFloat("material.specular_intensity", 1.0f);
	standard_mat->SetVec3("globalAmbientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	standard_mat->setFloat("globalAmbientIntensity", 0.3f);
	standard_mat->setTexture("material.diffuse", Game_Resources::Textures::CONTAINER_DIFFUSE);
	standard_mat->setTexture("material.specular", Game_Resources::Textures::CONTAINER_SPECULAR);

	/*floor_mat = new Standard_Material();
	floor_mat->SetVec3("material.ambientColor", glm::vec3(1.0f, 0.5f, 0.31f));
	floor_mat->SetVec3("material.diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
	floor_mat->SetVec2("material.scale", glm::vec2(100.0f, 100.0f));
	floor_mat->setFloat("material.shininess", 32.0f);
	floor_mat->setFloat("material.specular_intensity", 1.0f);
	floor_mat->SetVec3("globalAmbientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	floor_mat->setFloat("globalAmbientIntensity", 0.3f);
	floor_mat->setTexture("material.diffuse", Game_Resources::Textures::CONTAINER_DIFFUSE);
	floor_mat->setTexture("material.specular", Game_Resources::Textures::CONTAINER_SPECULAR);*/

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
	Camera_obj->Get_Transform()->Position(glm::vec3(0, 0, 10));
	camera = Camera_obj->Add_Component<Camera>();
	//camera->Clear_Color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	camera->FOV(90.0f);
	//camera->Set_Skybox(skybox_cubmap);

	glm::vec4 light_color_dir = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	glm::vec3 light_pos_dir = glm::vec3(0.0f, 0.0f, 100.0f);
	WorldObject* light_obj_dir;
	Light* light_comp_dir;
	create_light_object(&light_obj_dir, &light_comp_dir, Light::Light_Type::DIRECTIONAL, light_pos_dir, 1, light_color_dir);
	light_comp_dir->Enabled(true);
	light_obj_dir->Get_Transform()->LookAt(glm::vec3(10.0f, -50.0f, -20.0f));

	std::vector<WorldObject*> objects;
	for (int i = 0; i < cubePositions.size(); i++) {
		WorldObject* obj = Instantiate("test_object");
		obj->Get_MeshRenderer()->Set_Mesh(cube_mesh);
		//obj->Get_MeshRenderer()->Set_Shader(m_shader);
		obj->Get_MeshRenderer()->Set_Material(standard_mat);

		obj->Get_Transform()->Translate(cubePositions[i].x, cubePositions[i].y, cubePositions[i].z);
		objects.push_back(obj);
	}

	//WorldObject* floor_obj = Instantiate("test_object");
	//floor_obj->Get_MeshRenderer()->Set_Mesh(cube_mesh);
	//obj->Get_MeshRenderer()->Set_Shader(m_shader);
	//floor_obj->Get_MeshRenderer()->Set_Material(floor_mat);

	//floor_obj->Get_Transform()->Translate(0.0f, -5.0f, 0.0f);
	//floor_obj->Get_Transform()->Scale(glm::vec3(100.0f, 1.0f, 100.0f));
	
}

void Test_Scene::Update(float dt)
{
	
	//Camera_obj->Get_Transform()->Rotate(glm::vec3(0, 25.0f * dt, 0.0f));


}

void Test_Scene::create_light_object(WorldObject** obj, Light** light_comp, Light::Light_Type type, glm::vec3 pos, float scale, glm::vec4 color)
{
	*obj = new WorldObject("light");
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
