#include "Voxel_Test_Scene.h"

#include "Standard_Material.h"
#include "Opaque_Chunk_Material.h"
#include "Game_Resources.h"
#include "Cubemap.h"

#include "GPUSort.h"

#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>  // For rand() and srand()
#include <time.h>    // For time()

#include "Editor_Camera_Control.h"

using namespace VoxelEngine;
using namespace input;

void Voxel_Test_Scene::Init()
{
	Logger::LogInfo(LOG_POS("Init"), "Test Scene started");

	std::vector<glm::vec3> floor_vertices = {
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

	std::vector<glm::vec2> floor_tex_coords = {
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

	std::vector<glm::vec3> floor_normals = {
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

	glm::vec4 cube_color(1.0f, 1.0f, 1.0f, 1.0f);
	std::vector<glm::vec4> floor_cube_colors;
	floor_cube_colors.assign(floor_vertices.size(), cube_color);



	// Create Directional light
	glm::vec4 light_color_dir = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 light_pos_dir = glm::vec3(0.0f, 0.0f, 100.0f);
	create_light_object(&light_obj_dir, &light_comp_dir, Light::Light_Type::DIRECTIONAL, light_pos_dir, 1, light_color_dir);
	light_comp_dir->Enabled(true);
	light_obj_dir->Get_Transform()->LookAt(glm::vec3(10.0f, -50.0f, -20.0f));

	light_comp_dir->Strength(1.0f);

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
	Camera_obj->Get_Transform()->Position(glm::vec3(0, 5, 6));
	Camera_obj->Rotate(-25.0f, 0.0f, 0.0f);
	camera = Camera_obj->Add_Component<Camera>();
	Camera_obj->Add_Component<Editor_Camera_Control>();
	//camera->Clear_Color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//camera->FOV(90.0f);
	camera->Set_Skybox(skybox_cubmap);

	standard_mat = new Standard_Material();
	standard_mat->SetVec3("material.ambientColor", glm::vec3(1.0f, 0.5f, 0.31f));
	standard_mat->SetVec3("material.diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
	standard_mat->SetVec2("material.scale", glm::vec2(32.0f, 32.0f));
	standard_mat->setFloat("material.shininess", 32.0f);
	standard_mat->setFloat("material.specular_intensity", 1.0f);
	standard_mat->SetVec3("globalAmbientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	standard_mat->setFloat("globalAmbientIntensity", 0.1f);
	standard_mat->setTexture("material.diffuse", Game_Resources::Textures::CONTAINER_DIFFUSE);
	standard_mat->setTexture("material.specular", Game_Resources::Textures::CONTAINER_SPECULAR);

	chunk_opaque_mat = new Opaque_Chunk_Material();
	chunk_opaque_mat->SetVec3("material.ambientColor", glm::vec3(1.0f, 0.5f, 0.31f));
	chunk_opaque_mat->SetVec3("material.diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
	chunk_opaque_mat->SetVec2("material.scale", glm::vec2(1.0f, 1.0f));
	chunk_opaque_mat->setFloat("material.shininess", 32.0f);
	chunk_opaque_mat->setFloat("material.specular_intensity", 1.0f);
	chunk_opaque_mat->SetVec3("globalAmbientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	chunk_opaque_mat->setFloat("globalAmbientIntensity", 0.1f);


	Mesh* cube_mesh = new Mesh();
	cube_mesh->Vertices(floor_vertices);
	cube_mesh->Normals(floor_normals);
	cube_mesh->Colors(floor_cube_colors);
	cube_mesh->TexCoords(floor_tex_coords);

	WorldObject* floor_obj = Instantiate("floor");
	floor_obj->Get_MeshRenderer()->Set_Mesh(cube_mesh);
	floor_obj->Get_MeshRenderer()->Set_Material(standard_mat);
	floor_obj->Get_Transform()->Translate(16.0f, 0.0f, 16.0f);
	floor_obj->Get_Transform()->Scale(glm::vec3(32.0f, 1.0f, 32.0f));



	ChunkSettings settings;
	ChunkGenerationOptions gen_options;
	ChunkRenderOptions render_options;

	settings.GetSettings()->setString("programDir", std::string("C:/Users/jrurka/Source/repos/game_project/resources/shaders/compute/voxelEngine/Bin"));
	settings.GetSettings()->setFloat("voxelsPerMeter", 1);
	settings.GetSettings()->setInt("chunkMeterSizeX", 32);
	settings.GetSettings()->setInt("chunkMeterSizeY", 32);
	settings.GetSettings()->setInt("chunkMeterSizeZ", 32);
	settings.GetSettings()->setInt("TotalBatchGroups", 1);
	settings.GetSettings()->setInt("BatchesPerGroup", 1);
	settings.GetSettings()->setInt("InvertTrianges", false);

	m_builder = new SmoothVoxelBuilder();

	m_builder->Init(&settings);

	glm::ivec4 chunk_loc = glm::ivec4(0, 0, 0, 0);
	gen_options.locations.push_back(chunk_loc);
	render_options.locations.push_back(chunk_loc);

	glm::dvec4 gen_times = m_builder->Generate(&gen_options);
	glm::dvec4 render_times = m_builder->Render(&render_options);

	std::vector<glm::ivec4> counts = m_builder->GetSize();
	glm::ivec4 chnk_count = counts[0];

	glm::vec4* m_vertices = new glm::vec4[chnk_count.x];
	glm::vec4* m_normals = new glm::vec4[chnk_count.x];
	int* m_triangles = new int[chnk_count.x];

	m_builder->Extract(
		m_vertices,
		m_vertices,
		m_triangles,
		counts[0]
	);

	std::vector<glm::vec4> verts(m_vertices, m_vertices + chnk_count.x);
	std::vector<glm::vec4> normals(m_normals, m_normals + chnk_count.x);

	glm::vec4 max_val = glm::vec4(-1000, -1000, -1000, -1000);
	for (const auto& elem : verts) {
		max_val = glm::max(max_val, elem);
	}
	Logger::LogDebug(LOG_POS("Init"), "Maximum vertex: (%f, %f, %f)", max_val.x, max_val.y, max_val.z);


	Logger::LogDebug(LOG_POS("Init"), "Number of vertices: %i", (int)chnk_count.x);

	Mesh* voxel_mesh_test = new Mesh();
	voxel_mesh_test->Vertices(Utilities::vec4_to_vec3_arr(verts));
	voxel_mesh_test->Normals(Utilities::vec4_to_vec3_arr(normals));
	//voxel_mesh_test->Generate_Normals();


	WorldObject* obj = Instantiate("test_object");
	obj->Get_MeshRenderer()->Set_Mesh(voxel_mesh_test);
	//obj->Get_MeshRenderer()->Transparent(true);
	//obj->Get_MeshRenderer()->Set_Shader(m_shader);
	obj->Get_Transform()->Translate(16.0f, 16.0f, 16.0f);
	obj->Get_MeshRenderer()->Set_Material(chunk_opaque_mat);
	obj->Get_Transform()->Translate(0, 0, 0);

}

void Voxel_Test_Scene::Update(float dt)
{
	//light_obj_dir->Get_Transform()->Rotate(0.0f, 25.0f * dt, 0.0f);

	
}

void Voxel_Test_Scene::create_light_object(WorldObject** obj, Light** light_comp, Light::Light_Type type, glm::vec3 pos, float scale, glm::vec4 color)
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



