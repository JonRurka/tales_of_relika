#include "Test_Scene.h"

#include "Standard_Material.h"
#include "Game_Resources.h"
#include "Cubemap.h"

#include "GPUSort.h"

#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>  // For rand() and srand()
#include <time.h>    // For time()

#define PROGRAM_NAME "create_vbo"
#define KERNEL_NAME "main"


//#include <GL/glew.h>

/*namespace {
	int test_ext() {
		// Initialize GLEW after creating OpenGL context
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			return 1;
		}

		// Check for extension
		if (GLEW_EXT_memory_object) {
			printf("GL_EXT_memory_object is supported\n");
		}
		else {
			printf("GL_EXT_memory_object is not supported\n");
		}
		return 0;
	}
}*/

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

	const int duplicate = 0;
	std::vector<glm::vec3> cubePositions_orig = cubePositions;
	for (int i = 0; i < duplicate; i++) {
		cubePositions.insert(cubePositions.end(), cubePositions_orig.begin(), cubePositions_orig.end());
	}
	Logger::LogDebug(LOG_POS("Init"), "Num Cubes: %i", (int)cubePositions.size());

	glm::vec4 cube_color(1.0f, 1.0f, 1.0f, 1.0f);
	std::vector<glm::vec4> cube_colors;
	cube_colors.assign(vertices.size(), cube_color);

	Mesh* cube_mesh = new Mesh();
	cube_mesh->Vertices(vertices);
	cube_mesh->Normals(normals);
	cube_mesh->Colors(cube_colors);
	cube_mesh->TexCoords(tex_coords);



	Setup_gpu_program(vertices.size());

	in_vertices->SetData(Utilities::vec3_to_vec4_arr(vertices).data());
	in_normals->SetData(Utilities::vec3_to_vec4_arr(normals).data());
	in_colors->SetData(cube_colors.data());
	in_texcoords->SetData(Utilities::vec2_to_vec4_arr(tex_coords).data());

	Mesh* cube_mesh_test = new Mesh();//out_vbo->GetSize()
	Logger::LogDebug(LOG_POS("Init"), "Mumber of vertices: %i", (int)vertices.size());

	float start = Utilities::Get_Time();
	m_program->RunKernel(KERNEL_NAME, vertices.size(), 1, 1);

	cube_mesh_test->Name("GPU Mesh");
	cube_mesh_test->Load(out_vbo);
	float stop = Utilities::Get_Time();

	Logger::LogDebug(LOG_POS("Init"), "Mesh generated in: %f ms", (stop-start) * 1000);


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

	floor_mat = new Standard_Material();
	floor_mat->SetVec3("material.ambientColor", glm::vec3(1.0f, 0.5f, 0.31f));
	floor_mat->SetVec3("material.diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
	floor_mat->SetVec2("material.scale", glm::vec2(100.0f, 100.0f));
	floor_mat->setFloat("material.shininess", 32.0f);
	floor_mat->setFloat("material.specular_intensity", 1.0f);
	floor_mat->SetVec3("globalAmbientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	floor_mat->setFloat("globalAmbientIntensity", 0.3f);
	floor_mat->setTexture("material.diffuse", Game_Resources::Textures::CONTAINER_DIFFUSE);
	floor_mat->setTexture("material.specular", Game_Resources::Textures::CONTAINER_SPECULAR);

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

		obj->Get_MeshRenderer()->Set_Mesh(cube_mesh_test);

		obj->Get_MeshRenderer()->Transparent(true);
		//obj->Get_MeshRenderer()->Set_Shader(m_shader);
		obj->Get_MeshRenderer()->Set_Material(standard_mat);

		obj->Get_Transform()->Translate(cubePositions[i].x, cubePositions[i].y, cubePositions[i].z);
		objects.push_back(obj);
	}

	WorldObject* floor_obj = Instantiate("test_object");
	floor_obj->Get_MeshRenderer()->Set_Mesh(cube_mesh);
	floor_obj->Get_MeshRenderer()->Set_Material(floor_mat);
	floor_obj->Get_MeshRenderer()->Transparent(true);
	floor_obj->Get_Transform()->Translate(0.0f, -5.0f, 0.0f);
	floor_obj->Get_Transform()->Scale(glm::vec3(100.0f, 1.0f, 100.0f));


	







	/*std::vector<Renderer*> alpha_renderers;
	std::vector<glm::vec4> alpha_object_idx;
	alpha_renderers.reserve(Objects().size());
	alpha_object_idx.reserve(Objects().size());

	int i = 0;
	std::vector<unsigned int> shader_ids = Shader::Get_Shader_ID_List();
	for (const auto& ID : shader_ids) {
		Shader* shader = Shader::Get_Shader(ID);
		shader->use(true);
		std::vector<Renderer*> renderers = Shader::Get_Shader_Renderer_List(ID);
		for (const auto& rend : renderers) {
			if (rend->Transparent()) {
				alpha_renderers.push_back(rend);
				float dist = glm::distance(Camera_obj->Get_Transform()->Position(), rend->worldObject()->Get_Transform()->Position());
				glm::vec4 a_map = glm::vec4((float)i++, dist, 0.0f, 0.0f);
				alpha_object_idx.push_back(a_map);
				continue;
			}
		}
	}

	GPUSort* sort = new GPUSort(1024);
	sort->Sort(alpha_object_idx, false);

	for (const auto& elem : alpha_object_idx) {
		Logger::LogDebug(LOG_POS("Init"), "%f: %f", elem.x, elem.y);
	}*/


	/*if (Utilities::Is_Extension_Supported("GL_EXT_memory_object")) {
		printf("GL_EXT_memory_object is supported\n");
	}
	else {
		printf("GL_EXT_memory_object is not supported\n");
	}*/


	/*const int num_test = 10;
	glm::vec4* list = new glm::vec4[num_test];
	memset(list, 0, sizeof(float) * 4);

	srand(time(NULL));
	for (int i = 0; i < num_test; i++) {
		//list.push_back(glm::vec4(i, num_test - i, 0.0f, 0.0f));

		list[i] = glm::vec4(i, rand() % num_test, 0.0f, 0.0f);
		Logger::LogDebug(LOG_POS("Init"), "%f, %f", list[i].x, list[i].y);
	}

	GPUSort* sort = new GPUSort(num_test);
	sort->Sort(list, num_test, false);

	for (int i = 0; i < num_test; i++) {
		Logger::LogDebug(LOG_POS("Init"), "%f, %f", list[i].x, list[i].y);
	}*/
	
}

void Test_Scene::Update(float dt)
{
	m_timer += dt;

	if (m_timer > 1.0f) {
		//Logger::LogDebug(LOG_POS("Update"), "FPS: %f", Engine::FPS());
		m_timer = 0;
	}
	
	//Camera_obj->Get_Transform()->Rotate(glm::vec3(0, 25.0f * dt, 0.0f));
	//out_vbo->Dispose();
	//delete out_vbo;
	//out_vbo = m_controller->NewReadWriteBuffer(1000, sizeof(float) * 11, true);

}

void Test_Scene::create_light_object(WorldObject** obj, Light** light_comp, Light::Light_Type type, glm::vec3 pos, float scale, glm::vec4 color)
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

void Test_Scene::Setup_gpu_program(int size)
{
	std::vector<Vulkan_Device_Info> devices = ComputeInterface::GetSupportedDevices_Vulkan();
	m_device = devices[1];

	std::string shaderDir = "C:\\Users\\jrurka\\Source\\repos\\game_project\\resources\\shaders\\compute\\";

	m_controllerInfo.device = &m_device;
	m_controllerInfo.SetProgramDir(shaderDir);

	m_controller = ComputeInterface::GetComputeController(ComputeInterface::VULKAN, m_controllerInfo);

	IComputeProgram::ProgramInfo program_info = IComputeProgram::ProgramInfo(PROGRAM_NAME, IComputeProgram::FileType::Binary_File);
	program_info.AddKernel(KERNEL_NAME);

	m_program = m_controller->AddProgram(program_info);
	m_program->Build();
	//m_program->KernelSetWorkGroupSize(KERNEL_NAME, glm::uvec3(128, 1, 1));

	in_vertices = m_controller->NewReadBuffer(size, sizeof(float) * 4);
	in_normals = m_controller->NewReadBuffer(size, sizeof(float) * 4);
	in_colors = m_controller->NewReadBuffer(size, sizeof(float) * 4);
	in_texcoords = m_controller->NewReadBuffer(size, sizeof(float) * 4);

	out_vbo = m_controller->NewReadWriteBuffer(size, sizeof(float) * 11, true);
	
	IComputeProgram::BindIndex ind{};
	ind.GlobalIndex = 0;
	m_program->KernelSetBuffer(KERNEL_NAME, in_vertices, ind);

	ind = {};
	ind.GlobalIndex = 1;
	m_program->KernelSetBuffer(KERNEL_NAME, in_normals, ind);

	ind = {};
	ind.GlobalIndex = 2;
	m_program->KernelSetBuffer(KERNEL_NAME, in_colors, ind);

	ind = {};
	ind.GlobalIndex = 3;
	m_program->KernelSetBuffer(KERNEL_NAME, in_texcoords, ind);

	ind = {};
	ind.GlobalIndex = 4;
	m_program->KernelSetBuffer(KERNEL_NAME, out_vbo, ind);

	m_program->FinishBuild();
}
