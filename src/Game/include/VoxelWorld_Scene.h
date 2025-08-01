#pragma once

#include "game_engine.h"
#include "dynamic_compute.h"

#include "Network/Data.h"

class WorldGenController;
class StructureController;
class Client_Server;
class GameClient;
class NetPlayerManager;
class LocalPlayerCharacter;
class Item_Loader;
class Item_Type;

class VoxelWorld_Scene : public Scene {
public:

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

	WorldObject* light_obj_dir{ nullptr };
	Light* light_comp_dir{ nullptr };
	WorldObject* Camera_obj{ nullptr };
	Camera* camera{ nullptr };

	Item_Loader* m_item_loader{ nullptr };

	WorldObject* world_gen_controller_obj{ nullptr };
	WorldGenController* world_gen_controller{ nullptr };

	WorldObject* structure_controller_obj{ nullptr };
	StructureController* structure_controller{ nullptr };

	WorldObject* client_server_obj{ nullptr };
	Client_Server* client_server{ nullptr };

	WorldObject* game_client_obj{ nullptr };
	GameClient* game_client{ nullptr };

	WorldObject* local_player_character_obj{ nullptr };
	LocalPlayerCharacter* local_player_character{ nullptr };

	WorldObject* net_player_manager_obj{ nullptr };
	NetPlayerManager* net_player_manager{ nullptr };

	void setup_camera();

	void setup_lights();

	void setup_chunk_gen(json world_data);

	void setup_structure_controller(json world_data);

	void setup_client_server();

	void setup_game_client();

	void setup_local_player(json player_data);

	void setup_net_player_manager();

	void create_test_items();

	void create_light_object(WorldObject** obj, Light** light_comp, Light::Light_Type type, glm::vec3 pos, float scale, glm::vec4 color);

	inline static const std::string LOG_LOC{ "VOXELWORLD_SCENE" };
};