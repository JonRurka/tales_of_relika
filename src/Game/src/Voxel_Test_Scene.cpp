#include "Voxel_Test_Scene.h"

#include "Standard_Material.h"
#include "Opaque_Chunk_Material.h"
#include "Game_Resources.h"
#include "Cubemap.h"
#include "Physics.h"
#include "BoxCollider.h"
#include "MeshCollider.h"

#include "GPUSort.h"

#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>  // For rand() and srand()
#include <time.h>    // For time()

#include <chrono>

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
	//Camera_obj->Get_Transform()->Position(glm::vec3(0, 5, 6));
	Camera_obj->Get_Transform()->Position(glm::vec3(0, 10, -50));
	Camera_obj->Get_Transform()->LookAt(glm::vec3(0.0f, 10.0f, 100.0f));
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

	btVector3 min, max;

	WorldObject* floor_obj = Instantiate("floor");
	floor_obj->Get_MeshRenderer()->Set_Mesh(cube_mesh);
	floor_obj->Get_MeshRenderer()->Set_Material(standard_mat);
	floor_obj->Get_Transform()->Translate(16.0f, 0.0f, 16.0f);
	floor_obj->Get_Transform()->Scale(glm::vec3(32.0f, 1.0f, 32.0f));
	BoxCollider* col = floor_obj->Add_Component<BoxCollider>();
	col->Size(glm::vec3(16.0f, 0.5f, 16.0f));
	col->Mass(0.0f);
	col->Activate();
	col->RigidBody()->forceActivationState(DISABLE_DEACTIVATION);
	col->RigidBody()->getAabb(min, max);
	//col->RigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_STATIC_OBJECT);
	//col->RigidBody()->setUserIndex(-1);

	Logger::LogDebug(LOG_POS("Init"), "Floor Min:(%f, %f, %f), max:(%f, %f, %f)",
		min.x(), min.y(), min.z(), max.x(), max.y(), max.z());

	WorldObject* box_obj = Instantiate("box");
	box_obj->Get_MeshRenderer()->Set_Mesh(cube_mesh);
	box_obj->Get_MeshRenderer()->Set_Material(standard_mat);
	box_obj->Get_Transform()->Translate(5.0f, 5.0f, 10.0f);
	box_obj->Get_Transform()->Scale(glm::vec3(1.0f, 1.0f, 1.0f));
	BoxCollider* col_box = box_obj->Add_Component<BoxCollider>();
	col_box->Size(glm::vec3(0.5f, 0.5f, 0.5f));
	col_box->Mass(1.0f);
	col_box->Activate();
	col_box->RigidBody()->getAabb(min, max);
	
	Logger::LogDebug(LOG_POS("Init"), "Cube Min:(%f, %f, %f), max:(%f, %f, %f)",
		min.x(), min.y(), min.z(), max.x(), max.y(), max.z());

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

	//double start = Utilities::Get_Time();
	auto start = std::chrono::high_resolution_clock::now();

	glm::dvec4 gen_times = m_builder->Generate(&gen_options);
	glm::dvec4 render_times = m_builder->Render(&render_options);

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration<double>(end - start).count();
	//double stop = Utilities::Get_Time();

	std::vector<glm::ivec4> counts = m_builder->GetSize();
	glm::ivec4 chnk_count = counts[0];

	

	glm::vec4* m_vertices = new glm::vec4[chnk_count.x];
	glm::vec4* m_normals = new glm::vec4[chnk_count.x];
	unsigned int* m_triangles = new unsigned int[chnk_count.x];

	m_builder->Extract(
		m_vertices,
		m_normals,
		m_triangles,
		counts[0]
	);

	/*for (int i = 0; i < chnk_count.x; i++) {
		if (i % 5 == 0) {
			Graphics::DrawDebugRay(m_vertices[i], m_normals[i], glm::vec3(0, 0, 1), 1000.0f);
		}
	}*/

	
	Logger::LogDebug(LOG_POS("Init"), "Generate Time: %f", (duration) * 1000.0f);

	std::vector<glm::vec4> verts(m_vertices, m_vertices + chnk_count.x);
	std::vector<unsigned int> tris(m_triangles, m_triangles + chnk_count.x);
	std::vector<glm::vec4> normals(m_normals, m_normals + chnk_count.x);

	//glm::vec4 max_val = glm::vec4(-1000, -1000, -1000, -1000);
	float max_val = -1000;
	for (/*int i = 0; i < (int)chnk_count.x; i++ */ const auto& elem : verts) {
		//max_val = glm::max(max_val, elem);
		float dist = glm::distance(glm::vec4(0, 0, 0, 0), elem);
		max_val = glm::max(max_val, dist);
		//Logger::LogDebug(LOG_POS("Init"), "vertex: (%f, %f, %f)", m_vertices[i].x, m_vertices[i].y, m_vertices[i].z);
	}
	Logger::LogDebug(LOG_POS("Init"), "Maximum vertex dist: (%f)", max_val);


	Logger::LogDebug(LOG_POS("Init"), "Number of vertices: %i", (int)chnk_count.x);

	Mesh* voxel_mesh_test = new Mesh();
	voxel_mesh_test->Vertices(Utilities::vec4_to_vec3_arr(verts));
	voxel_mesh_test->Indices(tris);
	voxel_mesh_test->Normals(Utilities::vec4_to_vec3_arr(normals));
	//voxel_mesh_test->Generate_Normals();

	WorldObject* obj = Instantiate("test_voxel_object");
	obj->Get_Transform()->Set_Verbos(true);
	obj->Get_MeshRenderer()->Set_Mesh(voxel_mesh_test);
	//obj->Get_MeshRenderer()->Transparent(true);
	//obj->Get_MeshRenderer()->Set_Shader(m_shader);
	obj->Get_Transform()->Translate(0.0f, 0.0f, 0.0f);
	obj->Get_MeshRenderer()->Set_Material(chunk_opaque_mat);

	Transform* obj_trans = obj->Get_Transform();
	Logger::LogDebug(LOG_POS("Init"), "Orig Obj Position:(%f, %f, %f)",
		obj_trans->Position().x, obj_trans->Position().y, obj_trans->Position().z);

	MeshCollider* mesh_col = obj->Add_Component<MeshCollider>();
	mesh_col->SetMesh(voxel_mesh_test);
	mesh_col->Activate();
	//BoxCollider* test_col = obj->Add_Component<BoxCollider>();
	//test_col->Size(glm::vec3(0.5f, 0.5f, 0.5f));
	//test_col->Mass(0.0f);
	//test_col->Activate();

	//Graphics::DrawDebugRay(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), 10.0f);

	std::string test_str = "this is a test string to compress.";
	std::vector<unsigned char> vec_uncompressed;
	vec_uncompressed.assign(test_str.begin(), test_str.end());


	float comp_start = (float)Utilities::Get_Time();
	std::vector<unsigned char> vec_compressed = Utilities::Compress(vec_uncompressed);
	vec_uncompressed = Utilities::Decompress(vec_compressed);
	float comp_stop = (float)Utilities::Get_Time();

	std::string test_str_res(vec_uncompressed.begin(), vec_uncompressed.end());

	Logger::LogDebug(LOG_POS("Init"), "Compressed string (%f ms): %s", (comp_stop - comp_start) * 1000.0f, test_str_res.c_str());


	Logger::LogDebug(LOG_POS("Init"), "Data directory: %s", Resources::Get_Data_Director().c_str());
}

void Voxel_Test_Scene::Update(float dt)
{
	//light_obj_dir->Get_Transform()->Rotate(0.0f, 25.0f * dt, 0.0f);

	glm::vec3 start(5.0f, 10.0f, 5.0f);
	glm::vec3 stop(6.0f, -10.0f, 5.0f);

	/*Graphics::DrawDebugLine(start, stop, glm::vec3(0.0f, 1.0f, 0.0f));
	Physics::RayHit hit = Physics::Raycast(start, stop - start);
	if (hit.did_hit) {
		//Logger::LogDebug(LOG_POS("Update"), "hit");
		Graphics::DrawDebugRay(hit.hit_point, hit.normal, glm::vec3(0.0f, 0.0f, 1.0f));
	}*/

	/*if (Input::GetMouseKeyDown(MouseButton::Left)) {
		glm::vec3 ray_start;
		glm::vec3 ray_dir;
		camera->ScreenPointToRay(Input::Get_Mouse_Position(), ray_start, ray_dir);
		//Graphics::DrawDebugRay(ray_start, ray_dir * 100.0f, glm::vec3(1.0f, 1.0f, 0.0f), 100.0f);
		Logger::LogDebug(LOG_POS("Update"), "start:(%f, %f, %f), dir:(%f, %f, %f)", 
			ray_start.x, ray_start.y, ray_start.z, ray_dir.x, ray_dir.y, ray_dir.z);

		Physics::RayHit hit = Physics::Raycast(ray_start, ray_dir * 100.0f);
		if (hit.did_hit) {
			//Logger::LogDebug(LOG_POS("Update"), "hit");
			//Graphics::DrawDebugLine(ray_start, hit.hit_point, glm::vec3(1.0f, 1.0f, 0.0f), 100.0f);
			Graphics::DrawDebugRay(hit.hit_point, hit.normal, glm::vec3(0.0f, 0.0f, 1.0f), 100.0f);
		}
	}*/

	glm::vec3 ray_start;
	glm::vec3 ray_dir;
	camera->ScreenPointToRay(Input::Get_Mouse_Position(), ray_start, ray_dir);
	Physics::RayHit hit = Physics::Raycast(ray_start, ray_dir * 100.0f);
	if (hit.did_hit) {
		Graphics::DrawDebugRay(hit.hit_point, hit.normal, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	//Logger::LogDebug(LOG_POS("Update"), "FPS: %f", Engine::FPS());
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



