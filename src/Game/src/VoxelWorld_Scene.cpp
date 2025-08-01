#include "VoxelWorld_Scene.h"

#include "Game_Resources.h"
#include "WorldGenController.h"
#include "StructureController.h"
#include "Editor_Camera_Control.h"
#include "GameClient.h"
#include "Client_Server_Interface.h"
#include "NetPlayerManager.h"
#include "LocalPlayerCharacter.h"
#include "Character_HUD.h"
#include "Primitives.h"

#include "Item_Loader.h"
#include "Item_Type.h"

#include "HashHelper.h"
#include "Network/NetClient.h"
#include "Network/OpCodes.h"
#include "Network/BufferUtils.h"

#include "Standard_Material.h"

#define SERVER_START_WAIT_TIME (4.0f)
#define SERVER_DATA_REQUEST_WAIT_TIME (2.0f)

void VoxelWorld_Scene::Init()
{
	m_remote_connection = false;

	setup_camera();
	setup_lights();
	setup_client_server();
	

	//json world_data;
	//setup_chunk_gen(world_data);

	//create_test_items();


	m_start_time = Utilities::Get_Time();
}

void VoxelWorld_Scene::Update(float dt)
{
	if (!m_server_started) {

		double cur_time = Utilities::Get_Time();
		if (cur_time - m_start_time > SERVER_START_WAIT_TIME)
		{
			setup_game_client();
			m_server_started = true;
		}

	}

	if (m_client_connected) {
		if (!m_init_data_requested) {
			if (Utilities::Get_Time() - m_connected_time > SERVER_DATA_REQUEST_WAIT_TIME) {
				m_init_data_requested = true;
				Logger::LogInfo(LOG_POS("Update"), "Requesting world player data...");
				game_client->Send_World(OpCodes::Server_World::Request_World_Player_Data);
			}
		}
	}



	Graphics::Update_Window_Title("Tales of Relica || FPS: " + std::to_string(Engine::FPS()));
	//Logger::LogDebug(LOG_POS("Update"), "update");
}

void VoxelWorld_Scene::GameConnected()
{
	Logger::LogInfo(LOG_POS("GameConnected"), "Game server connected successfully.");
	m_connected_time = Utilities::Get_Time();
	m_client_connected = true;
	//game_client->Send_World(OpCodes::Server_World::Request_World_Player_Data);



	//setup_net_player_manager();



	/*Standard_Material* m_character_material = new Standard_Material();
	m_character_material->SetVec3("material_ambientColor", glm::vec3(1.0f, 0.5f, 0.31f));
	m_character_material->SetVec3("material_diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_character_material->SetVec2("material_scale", glm::vec2(32.0f, 32.0f));
	m_character_material->setFloat("material_shininess", 32.0f);
	m_character_material->setFloat("material_specular_intensity", 1.0f);
	m_character_material->SetVec3("globalAmbientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_character_material->setFloat("globalAmbientIntensity", 0.1f);
	m_character_material->setTexture("material_diffuse", Game_Resources::Textures::CONTAINER_DIFFUSE);
	m_character_material->setTexture("material_specular", Game_Resources::Textures::CONTAINER_SPECULAR);

	WorldObject* obj = Instantiate("test_obj");

	glm::vec4 cube_color(1.0f, 1.0f, 1.0f, 1.0f);
	std::vector<glm::vec4> floor_cube_colors;
	floor_cube_colors.assign(Primitives::Capsule_Vertices.size(), cube_color);

	Mesh* cube_mesh = new Mesh();
	cube_mesh->Vertices(Primitives::Capsule_Vertices);
	cube_mesh->Normals(Primitives::Capsule_Normals);
	cube_mesh->Colors(floor_cube_colors);
	cube_mesh->Indices(Primitives::Capsule_Triangles);
	//cube_mesh->TexCoords(floor_tex_coords);
	cube_mesh->Activate();

	obj->Get_Transform()->Translate(0.0f, 10.0f, 0.0f);
	obj->Get_MeshRenderer()->Set_Mesh(cube_mesh);
	obj->Get_MeshRenderer()->Set_Material(m_character_material);*/
}

void VoxelWorld_Scene::OnWorldPlayerDataResult(Data data)
{
	Logger::LogInfo(LOG_POS("OnWorldPlayerDataResult"), "Received world player data.");

	std::string data_json_str = HashHelper::BytesToString(data.Buffer);
	json world_player_data = json::parse(data_json_str);

	json world_data = world_player_data["world"];
	json player_data = world_player_data["player"];

	setup_chunk_gen(world_data);
	setup_structure_controller(world_data);
	setup_local_player(player_data);
	setup_net_player_manager();

	world_gen_controller->Start();
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
	camera->Clear_Color(glm::vec4(1.0, 1.0, 0.0, 1.0));
	//Editor_Camera_Control* cam_control = Camera_obj->Add_Component<Editor_Camera_Control>();
	//cam_control->Speed(10.0f);
	//camera->Clear_Color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//camera->FOV(90.0f);
	camera->Set_Skybox(skybox_cubmap);

	//Character_HUD* hud = Camera_obj->Add_Component<Character_HUD>();
	//hud->Init(camera);


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

void VoxelWorld_Scene::setup_chunk_gen(json world_data)
{
	world_gen_controller_obj = Instantiate("World_Gen_Controller");
	world_gen_controller = world_gen_controller_obj->Add_Component<WorldGenController>();
}

void VoxelWorld_Scene::setup_structure_controller(json world_data)
{
	structure_controller_obj = Instantiate("Structure_Controller");
	structure_controller = structure_controller_obj->Add_Component<StructureController>();
}

void VoxelWorld_Scene::setup_client_server()
{
	client_server_obj = Instantiate("Client_Server");
	client_server = client_server_obj->Add_Component<Client_Server>();
	if (!m_remote_connection) {
		client_server->Initialize_Server();
	}
}

void VoxelWorld_Scene::setup_game_client()
{
	game_client_obj = Instantiate("Game_Client");
	game_client = game_client_obj->Add_Component<GameClient>();
	game_client->Init("test_user", 1, m_remote_connection);
	game_client->SetOnConnectSuccess(OnGameConnect, this);
	game_client->Net_Client()->AddCommand(OpCodes::Client::World_Player_Data_Result, OnWorldPlayerDataResult_cb, this);
	game_client->Connect();
	Logger::LogInfo(LOG_POS("setup_game_client"), "Connecting to game server...");
}

void VoxelWorld_Scene::setup_local_player(json player_data)
{
	m_item_loader = new Item_Loader();
	m_item_loader->Load_Items(Game_Resources::Data_Files::ITEM_TYPES);
	Item_Type::Init();

	json location_obj = player_data["location"];
	float x = location_obj["x"];
	float y = location_obj["y"];
	float z = location_obj["z"];
	glm::vec3 loc = glm::vec3(x, y, z);

	Logger::LogDebug(LOG_POS("setup_local_player"), "Received Pos: (%f, %f, %f)",
		loc.x, loc.y, loc.z);

	local_player_character_obj = Instantiate("Local_Character");
	local_player_character_obj->Get_Transform()->Position(loc);
	local_player_character = local_player_character_obj->Add_Component<LocalPlayerCharacter>();
	local_player_character->Set_Camera_Object(Camera_obj);

	world_gen_controller->SetTarget(local_player_character_obj->Get_Transform());
}

void VoxelWorld_Scene::setup_net_player_manager()
{
	net_player_manager_obj = Instantiate("Net_Player_Manager");
	net_player_manager = net_player_manager_obj->Add_Component<NetPlayerManager>();
	net_player_manager->RegisterLocalPlayer(local_player_character);

}

void VoxelWorld_Scene::create_test_items()
{
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

	Standard_Material* standard_mat = new Standard_Material();
	standard_mat->SetVec3("material_ambientColor", glm::vec3(1.0f, 0.5f, 0.31f));
	standard_mat->SetVec3("material_diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
	standard_mat->SetVec2("material_scale", glm::vec2(32.0f, 32.0f));
	standard_mat->setFloat("material_shininess", 32.0f);
	standard_mat->setFloat("material_specular_intensity", 1.0f);
	standard_mat->SetVec3("globalAmbientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	standard_mat->setFloat("globalAmbientIntensity", 0.1f);
	standard_mat->setTexture("material_diffuse", Game_Resources::Textures::CONTAINER_DIFFUSE);
	standard_mat->setTexture("material_specular", Game_Resources::Textures::CONTAINER_SPECULAR);

	Mesh* cube_mesh = new Mesh();
	cube_mesh->Vertices(floor_vertices);
	cube_mesh->Normals(floor_normals);
	cube_mesh->Colors(floor_cube_colors);
	cube_mesh->TexCoords(floor_tex_coords);
	cube_mesh->Activate();

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
