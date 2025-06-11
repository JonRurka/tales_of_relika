#pragma once

#include "game_engine.h"
#include "dynamic_compute.h"


class WorldGenController;
class Client_Server;
class GameClient;
class NetPlayerManager;
class LocalPlayerCharacter;

class VoxelWorld_Scene : public Scene {
public:

	static void OnGameConnect(void* obj) {
		VoxelWorld_Scene* game_client = (VoxelWorld_Scene*)obj;
		game_client->GameConnected();
	}
	void GameConnected();

protected:
	void Init() override;

	void Update(float dt) override;

private:

	double m_start_time{ 0 };

	bool m_remote_connection{ false };
	bool m_client_server_inited{ false };

	WorldObject* light_obj_dir{ nullptr };
	Light* light_comp_dir{ nullptr };
	WorldObject* Camera_obj{ nullptr };
	Camera* camera{ nullptr };

	WorldObject* world_gen_controller_obj{ nullptr };
	WorldGenController* world_gen_controller{ nullptr };

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

	void setup_chunk_gen();

	void setup_client_server();

	void setup_game_client();

	void setup_local_player();

	void setup_net_player_manager();

	void create_light_object(WorldObject** obj, Light** light_comp, Light::Light_Type type, glm::vec3 pos, float scale, glm::vec4 color);

	inline static const std::string LOG_LOC{ "VOXELWORLD_SCENE" };
};