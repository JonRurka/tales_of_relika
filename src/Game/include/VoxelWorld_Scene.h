#pragma once

#include "game_engine.h"
#include "dynamic_compute.h"

#include "Network/Data.h"

#include "WorldGenController.h"
#include "StructureController.h"
#include "Client_Server_Interface.h"
#include "GameClient.h"
#include "LocalPlayerCharacter.h"
#include "NetPlayerManager.h"

#include <memory>

class WorldGenController;
class StructureController;
class Client_Server;
class GameClient;
class NetPlayerManager;
class LocalPlayerCharacter;
class Item_Loader;
class Item_Type;

namespace Rml {
	class Context;
	class ElementDocument;
}

class VoxelWorld_Scene : public Scene {
public:

	enum class ConnectMode : int
	{
		Local = 1,
		Remote = 2
	};

	static void OnGameConnect(void* obj) {
		VoxelWorld_Scene* game_client = (VoxelWorld_Scene*)obj;
		game_client->GameConnected();
	}
	void GameConnected();

	static void OnWorldPlayerDataResult_cb(void* obj, Data data) {
		VoxelWorld_Scene* game_client = (VoxelWorld_Scene*)obj;
		game_client->OnWorldPlayerDataResult(data);
	}
	void OnWorldPlayerDataResult(Data data);

	bool Game_Ready();

	VoxelWorld_Scene();

protected:
	void Init() override;

	void Update(float dt) override;

private:

	double m_start_time{ 0 };

	bool m_remote_connection{ false };
	bool m_server_started{ false };
	bool m_client_connected{ false };
	bool m_init_data_requested{ false };

	double m_connected_time{ 0 };

	ElementDocument m_loading_screen;
	bool m_loading_hidden{ false };

	Cubemap::Shared m_skybox_cubmap;

	WorldObject::Weak light_obj_dir;
	Light::Weak light_comp_dir;
	WorldObject::Weak Camera_obj;
	Camera::Weak camera;

	WorldObject::Weak world_gen_controller_obj;
	WorldGenController::Weak world_gen_controller;

	WorldObject::Weak structure_controller_obj;
	StructureController::Weak structure_controller;

	WorldObject::Weak client_server_obj;
	Client_Server::Weak client_server;

	WorldObject::Weak game_client_obj;
	GameClient::Weak game_client;

	WorldObject::Weak local_player_character_obj;
	LocalPlayerCharacter::Weak local_player_character;

	WorldObject::Weak net_player_manager_obj;
	NetPlayerManager::Weak net_player_manager;

	void startup_squence();

	void setup_camera();

	void setup_lights();

	void setup_chunk_gen(json world_data);

	void setup_structure_controller(json world_data);

	void setup_client_server();

	void setup_game_client();

	void setup_local_player(json player_data);

	void setup_net_player_manager();

	void create_test_items();

	inline static const std::string LOG_LOC{ "VOXELWORLD_SCENE" };
};