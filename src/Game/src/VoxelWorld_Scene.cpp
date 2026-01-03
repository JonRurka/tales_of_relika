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

#include <RmlUi/Core.h>
#include "UI_Engine.h"

#define SERVER_START_WAIT_TIME (4.0f)
#define SERVER_DATA_REQUEST_WAIT_TIME (2.0f)

void VoxelWorld_Scene::Init()
{
	VoxelWorld_Scene::ConnectMode connect_type = (VoxelWorld_Scene::ConnectMode)Start_Data().getInt("connection");
	m_remote_connection = (connect_type == VoxelWorld_Scene::ConnectMode::Remote);

	m_loading_screen = UI_Engine::Instance().Load_Document_Resource("loading", Game_Resources::UI::Documents::HUD::LOADING);
	m_loading_screen->Show();

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
	startup_squence();

	Graphics::Update_Window_Title("Tales of Relica || FPS: " + std::to_string(Engine::FPS()));
	//Logger::LogDebug(LOG_POS("Update"), "update");
}

void VoxelWorld_Scene::Deactivate()
{
	UI_Engine::Instance().Unload_Document("loading");
	m_loading_screen = nullptr;
}

void VoxelWorld_Scene::startup_squence()
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
				game_client.lock()->Send_World(OpCodes::Server_World::Request_World_Player_Data);
			}
		}
	}

	if (!m_loading_hidden)
	{
		if (Game_Ready())
		{
			m_loading_screen->Hide();
			m_loading_hidden = true;
		}
	}

}

// STEP 1: Connect
void VoxelWorld_Scene::setup_game_client()
{
	std::string username = Start_Data().getString("username");
	std::string host = Start_Data().getString("host");
	uint32_t user_id = Start_Data().getInt("user_id");

	if (username == "")
		username = "test_user";

	game_client_obj = Instantiate("Game_Client");
	game_client = game_client_obj.lock()->Add_Component<GameClient>();
	game_client.lock()->Init(username, host, user_id, m_remote_connection);
	game_client.lock()->SetOnConnectSuccess(OnGameConnect, this);
	game_client.lock()->Net_Client().AddCommand(OpCodes::Client::World_Player_Data_Result, OnWorldPlayerDataResult_cb, this);
	game_client.lock()->Connect();
	Logger::LogInfo(LOG_POS("setup_game_client"), "Connecting to game server...");
}

// STEP 2: Process connected result, and trigger count down to request player data.
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

// STEP 3: Receive player data, and initialize world
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

	world_gen_controller.lock()->Start();
}

// STEP 3a
void VoxelWorld_Scene::setup_chunk_gen(json world_data)
{
	world_gen_controller_obj = Instantiate("World_Gen_Controller");
	world_gen_controller = world_gen_controller_obj.lock()->Add_Component<WorldGenController>();
}

// STEP 3b
void VoxelWorld_Scene::setup_structure_controller(json world_data)
{
	structure_controller_obj = Instantiate("Structure_Controller");
	structure_controller = structure_controller_obj.lock()->Add_Component<StructureController>();
}

// STEP 3c
void VoxelWorld_Scene::setup_local_player(json player_data)
{
	Item_Loader::Instance().Load_Items(Game_Resources::Data_Files::ITEM_TYPES);
	Item_Type::Init();

	json location_obj = player_data["location"];
	float x = location_obj["x"];
	float y = location_obj["y"];
	float z = location_obj["z"];
	glm::vec3 loc = glm::vec3(x, y, z);

	Logger::LogDebug(LOG_POS("setup_local_player"), "Received Pos: (%f, %f, %f)",
		loc.x, loc.y, loc.z);

	local_player_character_obj = Instantiate("Local_Character");
	local_player_character_obj.lock()->Get_Transform().Position(loc);
	local_player_character = local_player_character_obj.lock()->Add_Component<LocalPlayerCharacter>();
	local_player_character.lock()->Set_Camera_Object(Camera_obj);

	world_gen_controller.lock()->SetTarget(local_player_character_obj.lock()->Get_Transform_Ptr());
}

// STEP 3d
void VoxelWorld_Scene::setup_net_player_manager()
{
	net_player_manager_obj = Instantiate("Net_Player_Manager");
	net_player_manager = net_player_manager_obj.lock()->Add_Component<NetPlayerManager>();
	net_player_manager.lock()->RegisterLocalPlayer(local_player_character);

}



bool VoxelWorld_Scene::Game_Ready()
{
	if (!m_server_started)
		return false;
	
	if (!m_client_connected)
		return false;

	if (world_gen_controller.expired())
		return false;

	if (!world_gen_controller.lock()->Terrain_Ready())
		return false;

	return true;
}

VoxelWorld_Scene::VoxelWorld_Scene()
{
	Logger::LogDebug(LOG_POS("NEW"), "Creat VoxelWorld Scene.");
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
	m_skybox_cubmap = Cubemap::Create(faces, false);
	Camera_obj = Instantiate("camera");
	//Camera_obj->Get_Transform()->Position(glm::vec3(0, 5, 6));
	Camera_obj.lock()->Get_Transform().Position(glm::vec3(0, 10, -50));
	Camera_obj.lock()->Get_Transform().LookAt(glm::vec3(0.0f, 10.0f, 100.0f));
	camera = Camera_obj.lock()->Add_Component<Camera>();
	camera.lock()->Clear_Color(glm::vec4(1.0, 1.0, 0.0, 1.0));


	//Editor_Camera_Control* cam_control = Camera_obj->Add_Component<Editor_Camera_Control>();
	//cam_control->Speed(10.0f);
	//camera->Clear_Color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//camera->FOV(90.0f);
	camera.lock()->Set_Skybox(m_skybox_cubmap);

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
	
	glm::vec4 light_color_dir = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec3 light_pos_dir = glm::vec3(0.0f, 100.0f, 0.0f);

	light_obj_dir = Instantiate("light");
	light_obj_dir.lock()->Get_Transform().Translate(light_pos_dir);
	light_obj_dir.lock()->Get_Transform().LookAt(glm::vec3(0.0f, -100.0f, -0.0f));

	light_comp_dir = light_obj_dir.lock()->Add_Component<Light>();
	light_comp_dir.lock()->Type(Light::Light_Type::DIRECTIONAL);
	light_comp_dir.lock()->Color(light_color_dir);
	light_comp_dir.lock()->Strength(2.0f);
	light_comp_dir.lock()->Linear_Coefficient(0.027f);
	light_comp_dir.lock()->Quadratic_Coefficient(0.0028f);
	light_comp_dir.lock()->CutOff(glm::cos(glm::radians(12.5f)));
	light_comp_dir.lock()->OuterCutOff(glm::cos(glm::radians(15.5f)));
}

void VoxelWorld_Scene::setup_client_server()
{
	client_server_obj = Instantiate("Client_Server");
	client_server = client_server_obj.lock()->Add_Component<Client_Server>();
	if (!m_remote_connection) {
		client_server.lock()->Initialize_Server();
	}
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

	Standard_Material::Shared standard_mat = std::make_shared<Standard_Material>();
	standard_mat->Set_Shader(Shader::Get_Shader("standard"));
	standard_mat->SetVec3("material_ambientColor", glm::vec3(1.0f, 0.5f, 0.31f));
	standard_mat->SetVec3("material_diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
	standard_mat->SetVec2("material_scale", glm::vec2(32.0f, 32.0f));
	standard_mat->setFloat("material_shininess", 32.0f);
	standard_mat->setFloat("material_specular_intensity", 1.0f);
	standard_mat->SetVec3("globalAmbientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	standard_mat->setFloat("globalAmbientIntensity", 0.1f);
	standard_mat->setTexture("material_diffuse", Game_Resources::Textures::CONTAINER_DIFFUSE);
	standard_mat->setTexture("material_specular", Game_Resources::Textures::CONTAINER_SPECULAR);

	Mesh::Shared cube_mesh = Mesh::Create();
	cube_mesh->Vertices(floor_vertices);
	cube_mesh->Normals(floor_normals);
	cube_mesh->Colors(floor_cube_colors);
	cube_mesh->TexCoords(floor_tex_coords);
	cube_mesh->Activate();

	//btVector3 min, max;

	WorldObject::Weak floor_obj = Instantiate("floor");
	floor_obj.lock()->Get_MeshRenderer().Set_Mesh(cube_mesh);
	floor_obj.lock()->Get_MeshRenderer().Set_Material(standard_mat);
	floor_obj.lock()->Get_Transform().Translate(16.0f, 0.0f, 16.0f);
	floor_obj.lock()->Get_Transform().Scale(glm::vec3(32.0f, 1.0f, 32.0f));
	BoxCollider::Weak col = floor_obj.lock()->Add_Component<BoxCollider>();
	col.lock()->Size(glm::vec3(16.0f, 0.5f, 16.0f));
	col.lock()->Mass(0.0f);
	col.lock()->Activate();
	//col.lock()->RigidBody().forceActivationState(DISABLE_DEACTIVATION);
	//col.lock()->RigidBody().getAabb(min, max);
	//col->RigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_STATIC_OBJECT);
	//col->RigidBody()->setUserIndex(-1);

	//Logger::LogDebug(LOG_POS("Init"), "Floor Min:(%f, %f, %f), max:(%f, %f, %f)",
	//	min.x(), min.y(), min.z(), max.x(), max.y(), max.z());

}


