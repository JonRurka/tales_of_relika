#include "Voxel_Test_Scene.h"

#include "Standard_Material.h"
#include "Opaque_Chunk_Material.h"
#include "Game_Resources.h"
#include "Cubemap.h"
#include "Physics.h"
#include "BoxCollider.h"
#include "MeshCollider.h"

#include "GPUSort.h"
#include "Stitch_VBO.h"

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

	std::vector<glm::vec4> floor_vertices = {
		glm::vec4(-0.5f, -0.5f, -0.5f, 0.0f),
		glm::vec4(0.5f, -0.5f,  -0.5f, 0.0f),
		glm::vec4(0.5f,  0.5f,  -0.5f, 0.0f),
		glm::vec4(0.5f,  0.5f,  -0.5f, 0.0f),
		glm::vec4(-0.5f,  0.5f, -0.5f, 0.0f),
		glm::vec4(-0.5f, -0.5f, -0.5f, 0.0f),

		glm::vec4(-0.5f, -0.5f,  0.5f, 0.0f),
		glm::vec4(0.5f, -0.5f,   0.5f, 0.0f),
		glm::vec4(0.5f,  0.5f,   0.5f, 0.0f),
		glm::vec4(0.5f,  0.5f,   0.5f, 0.0f),
		glm::vec4(-0.5f,  0.5f,  0.5f, 0.0f),
		glm::vec4(-0.5f, -0.5f,  0.5f, 0.0f),

		glm::vec4(-0.5f,  0.5f,  0.5f, 0.0f),
		glm::vec4(-0.5f,  0.5f, -0.5f, 0.0f),
		glm::vec4(-0.5f, -0.5f, -0.5f, 0.0f),
		glm::vec4(-0.5f, -0.5f, -0.5f, 0.0f),
		glm::vec4(-0.5f, -0.5f,  0.5f, 0.0f),
		glm::vec4(-0.5f,  0.5f,  0.5f, 0.0f),

		glm::vec4(0.5f,  0.5f,   0.5f, 0.0f),
		glm::vec4(0.5f,  0.5f,  -0.5f, 0.0f),
		glm::vec4(0.5f, -0.5f,  -0.5f, 0.0f),
		glm::vec4(0.5f, -0.5f,  -0.5f, 0.0f),
		glm::vec4(0.5f, -0.5f,   0.5f, 0.0f),
		glm::vec4(0.5f,  0.5f,   0.5f, 0.0f),

		glm::vec4(-0.5f, -0.5f, -0.5f, 0.0f),
		glm::vec4(0.5f, -0.5f,  -0.5f, 0.0f),
		glm::vec4(0.5f, -0.5f,   0.5f, 0.0f),
		glm::vec4(0.5f, -0.5f,   0.5f, 0.0f),
		glm::vec4(-0.5f, -0.5f,  0.5f, 0.0f),
		glm::vec4(-0.5f, -0.5f, -0.5f, 0.0f),

		glm::vec4(-0.5f,  0.5f, -0.5f, 0.0f),
		glm::vec4(0.5f,  0.5f,  -0.5f, 0.0f),
		glm::vec4(0.5f,  0.5f,   0.5f, 0.0f),
		glm::vec4(0.5f,  0.5f,   0.5f, 0.0f),
		glm::vec4(-0.5f,  0.5f,  0.5f, 0.0f),
		glm::vec4(-0.5f,  0.5f, -0.5f, 0.0f)
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

	std::vector<glm::vec4> floor_normals = {
		glm::vec4(0.0f,  0.0f, -1.0f, 0.0f),
		glm::vec4(0.0f,  0.0f, -1.0f, 0.0f),
		glm::vec4(0.0f,  0.0f, -1.0f, 0.0f),
		glm::vec4(0.0f,  0.0f, -1.0f, 0.0f),
		glm::vec4(0.0f,  0.0f, -1.0f, 0.0f),
		glm::vec4(0.0f,  0.0f, -1.0f, 0.0f),

		glm::vec4(0.0f,  0.0f,  1.0f, 0.0f),
		glm::vec4(0.0f,  0.0f,  1.0f, 0.0f),
		glm::vec4(0.0f,  0.0f,  1.0f, 0.0f),
		glm::vec4(0.0f,  0.0f,  1.0f, 0.0f),
		glm::vec4(0.0f,  0.0f,  1.0f, 0.0f),
		glm::vec4(0.0f,  0.0f,  1.0f, 0.0f),

		glm::vec4(-1.0f,  0.0f, 0.0f, 0.0f),
		glm::vec4(-1.0f,  0.0f, 0.0f, 0.0f),
		glm::vec4(-1.0f,  0.0f, 0.0f, 0.0f),
		glm::vec4(-1.0f,  0.0f, 0.0f, 0.0f),
		glm::vec4(-1.0f,  0.0f, 0.0f, 0.0f),
		glm::vec4(-1.0f,  0.0f, 0.0f, 0.0f),

		glm::vec4(1.0f,  0.0f,  0.0f, 0.0f),
		glm::vec4(1.0f,  0.0f,  0.0f, 0.0f),
		glm::vec4(1.0f,  0.0f,  0.0f, 0.0f),
		glm::vec4(1.0f,  0.0f,  0.0f, 0.0f),
		glm::vec4(1.0f,  0.0f,  0.0f, 0.0f),
		glm::vec4(1.0f,  0.0f,  0.0f, 0.0f),

		glm::vec4(0.0f, -1.0f,  0.0f, 0.0f),
		glm::vec4(0.0f, -1.0f,  0.0f, 0.0f),
		glm::vec4(0.0f, -1.0f,  0.0f, 0.0f),
		glm::vec4(0.0f, -1.0f,  0.0f, 0.0f),
		glm::vec4(0.0f, -1.0f,  0.0f, 0.0f),
		glm::vec4(0.0f, -1.0f,  0.0f, 0.0f),

		glm::vec4(0.0f,  1.0f,  0.0f, 0.0f),
		glm::vec4(0.0f,  1.0f,  0.0f, 0.0f),
		glm::vec4(0.0f,  1.0f,  0.0f, 0.0f),
		glm::vec4(0.0f,  1.0f,  0.0f, 0.0f),
		glm::vec4(0.0f,  1.0f,  0.0f, 0.0f),
		glm::vec4(0.0f,  1.0f,  0.0f, 0.0f)
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

	Texture* m_diffuse_texture_array = Texture::Create_Texture2D_Array({
		Game_Resources::Textures::Natural::DIRT_1_DIFFUSE,
		Game_Resources::Textures::Natural::GRASS_1_DIFFUSE,
		Game_Resources::Textures::Natural::STONE_1_DIFFUSE
	});

	standard_mat = new Standard_Material();
	standard_mat->SetVec3("material_ambientColor", glm::vec3(1.0f, 0.5f, 0.31f));
	standard_mat->SetVec3("material_diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
	standard_mat->SetVec2("material_scale", glm::vec2(32.0f, 32.0f));
	standard_mat->setFloat("material_shininess", 32.0f);
	standard_mat->setFloat("material_specular_intensity", 1.0f);
	standard_mat->SetVec3("globalAmbientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	standard_mat->setFloat("globalAmbientIntensity", 0.1f);
	standard_mat->setTexture("material_diffuse", Game_Resources::Textures::CONTAINER_DIFFUSE);
	standard_mat->setTexture("material_specular", Game_Resources::Textures::CONTAINER_SPECULAR);

	chunk_opaque_mat = new Opaque_Chunk_Material();
	chunk_opaque_mat->setTexture("diffuse", m_diffuse_texture_array);
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

	int max_vert = (int)Utilities::Vertex_Limit_Mode::Chunk_Max;

	Stitch_VBO* vbo_stitch = new Stitch_VBO();
	vbo_stitch->Init(m_builder, max_vert);

	int num_chunks = 1;

	std::vector<Mesh*> chunk_meshes;
	for (int i = 0; i < num_chunks; i++) {
		Mesh* voxel_mesh_test = new Mesh(max_vert * Stitch_VBO::Stride());
		chunk_meshes.push_back(voxel_mesh_test);

		glm::ivec4 chunk_loc = glm::ivec4(i, 0, 0, 0);
		gen_options.locations.push_back(chunk_loc);
		render_options.locations.push_back(chunk_loc);
	}

	//double start = Utilities::Get_Time();
	auto start = std::chrono::high_resolution_clock::now();

	Logger::LogDebug(LOG_POS("Init"), "Do Generate...");
	glm::dvec4 gen_times = m_builder->Generate(&gen_options);
	Logger::LogDebug(LOG_POS("Init"), "Do Render...");
	glm::dvec4 render_times = m_builder->Render(&render_options);
	Logger::LogDebug(LOG_POS("Init"), "Render Finished");

	//double stop = Utilities::Get_Time();

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration<double>(end - start).count();


	//auto end = std::chrono::high_resolution_clock::now();
	//auto process_duration = std::chrono::duration<double>(end - start).count();

	

	start = std::chrono::high_resolution_clock::now();

	auto get_counts_start = std::chrono::high_resolution_clock::now();
	std::vector<glm::ivec4> counts = m_builder->GetSize();
	auto get_counts_end = std::chrono::high_resolution_clock::now();
	double get_counts_duration = std::chrono::duration<double>(get_counts_end - get_counts_start).count();
	//glm::ivec4 chnk_count = counts[0];


	auto vbo_process_start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < num_chunks; i++) {
		Logger::LogDebug(LOG_POS("Init"), "Do VBO Process...");
		vbo_stitch->Process(chunk_meshes[i], counts[i], false);
	}
	auto vbo_process_end = std::chrono::high_resolution_clock::now();
	double vbo_process_duration = std::chrono::duration<double>(vbo_process_end - vbo_process_start).count();


	end = std::chrono::high_resolution_clock::now();
	auto extract_duration = std::chrono::duration<double>(end - start).count();

	double set_buffer_time = gen_times.z + render_times.w;

	Logger::LogDebug(LOG_POS("Init"), "");
	Logger::LogDebug(LOG_POS("Init"), "Generate - Assemble: %f ms", gen_times.x * 1000);
	Logger::LogDebug(LOG_POS("Init"), "Generate - Construct: %f ms", gen_times.y * 1000);
	Logger::LogDebug(LOG_POS("Init"), "");

	Logger::LogDebug(LOG_POS("Init"), "Render - Mark: %f ms", render_times.x * 1000);
	Logger::LogDebug(LOG_POS("Init"), "Render - Mark Offsets: %f ms", render_times.y * 1000);
	Logger::LogDebug(LOG_POS("Init"), "Render - Stitch: %f ms", render_times.z * 1000);
	Logger::LogDebug(LOG_POS("Init"), "");
	
	double time_sum = (gen_times.y + gen_times.z + render_times.y + render_times.z);
	Logger::LogDebug(LOG_POS("Init"), "Sum Generate Time: %f", time_sum * 1000.0f);
	Logger::LogDebug(LOG_POS("Init"), "Actual Generate Time: %f", (duration) * 1000.0f);
	Logger::LogDebug(LOG_POS("Init"), "Unaccounted Time: %f", (duration - time_sum) * 1000.0f);
	Logger::LogDebug(LOG_POS("Init"), "");

	Logger::LogDebug(LOG_POS("Init"), "SetBuffer: %f ms", set_buffer_time * 1000);
	Logger::LogDebug(LOG_POS("Init"), "Extract: %f ms", extract_duration * 1000);
	Logger::LogDebug(LOG_POS("Init"), "Extract - Get Counts : %f ms", get_counts_duration * 1000);
	Logger::LogDebug(LOG_POS("Init"), "Extract - VBO Process : %f ms", vbo_process_duration * 1000);
	Logger::LogDebug(LOG_POS("Init"), "Total: %f ms", (duration + extract_duration) * 1000);

	double s_per_chunk = ((duration + extract_duration) / num_chunks);
	double chunks_per_second = 1.0 / s_per_chunk;

	Logger::LogDebug(LOG_POS("Init"), "Per Chunk: %f ms (%f chunks/second)", s_per_chunk * 1000, chunks_per_second);
	Logger::LogDebug(LOG_POS("Init"), "");

	vbo_stitch->Reset();

	/*for (int i = 0; i < chnk_count.x; i++) {
		if (i % 5 == 0) {
			Graphics::DrawDebugRay(m_vertices[i], m_normals[i], glm::vec3(0, 0, 1), 1000.0f);
		}
	}*/

	//glm::vec4 max_val = glm::vec4(-1000, -1000, -1000, -1000);
	//float max_val = -1000;
	//for (/*int i = 0; i < (int)chnk_count.x; i++ */ const auto& elem : verts) {
		//max_val = glm::max(max_val, elem);
		//float dist = glm::distance(glm::vec4(0, 0, 0, 0), elem);
		//max_val = glm::max(max_val, dist);
		//Logger::LogDebug(LOG_POS("Init"), "vertex: (%f, %f, %f)", m_vertices[i].x, m_vertices[i].y, m_vertices[i].z);
	//}
	//Logger::LogDebug(LOG_POS("Init"), "Maximum vertex dist: (%f)", max_val);


	//Logger::LogDebug(LOG_POS("Init"), "Number of vertices: %i", (int)chnk_count.x);
	
	for (int i = 0; i < num_chunks; i++) {
		WorldObject* obj = Instantiate("test_voxel_object");
		obj->Get_Transform()->Set_Verbos(true);
		obj->Get_MeshRenderer()->Set_Mesh(chunk_meshes[i]);
		//obj->Get_MeshRenderer()->Transparent(true);
		//obj->Get_MeshRenderer()->Set_Shader(m_shader);
		obj->Get_Transform()->Translate(i * 32, 0.0f, 0.0f);
		obj->Get_MeshRenderer()->Set_Material(chunk_opaque_mat);
		MeshCollider* mesh_col = obj->Add_Component<MeshCollider>();
	}

	//Transform* obj_trans = obj->Get_Transform();
	//Logger::LogDebug(LOG_POS("Init"), "Orig Obj Position:(%f, %f, %f)",
	//	obj_trans->Position().x, obj_trans->Position().y, obj_trans->Position().z);

	

	//mesh_col->SetMesh(voxel_mesh_test);
	//mesh_col->Activate();


	//BoxCollider* test_col = obj->Add_Component<BoxCollider>();
	//test_col->Size(glm::vec3(0.5f, 0.5f, 0.5f));
	//test_col->Mass(0.0f);
	//test_col->Activate();

	//Graphics::DrawDebugRay(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), 10.0f);


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

	Graphics::Update_Window_Title("Tales of Relica || FPS: " + std::to_string(Engine::FPS()));
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



