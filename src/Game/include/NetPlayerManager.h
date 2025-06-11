#pragma once

#include "game_engine.h"

#include "Network/OpCodes.h"
#include "Network/Data.h"

#include <unordered_map>

class LocalPlayerCharacter;
class NetPlayerCharacter;

class NetPlayerManager : public Component {
public:

	struct PlayerCreationRequest {
	public:
		std::string UserName;
		uint32_t User_ID;
		uint16_t Instance_ID;
		glm::vec3 Position;
		glm::quat Rotation;
	};

	struct PlayerSpawnData {
		std::string UserName;
		uint32_t User_ID;
		int Instance_ID;
	};

	void RegisterLocalPlayer(LocalPlayerCharacter* local_player);

	void SetLocalUserID(uint32_t local_id) {
		m_local_player_id = local_id;
	}

	static void OnSpawnPlayers_cb(void* obj, Data data) {
		m_instance->OnUpdateOrientations(data);
	}
	void SpawnPlayers(Data data);

	static void OnUpdateOrientations_cb(void* obj, Data data) {
		m_instance->OnUpdateOrientations(data);
	}
	void OnUpdateOrientations(Data data);

	static void OnPlayerEvent_cb(void* obj, Data data) {
		m_instance->OnPlayerEvent(data);
	}
	void OnPlayerEvent(Data data);

	void RequestPlayers();

	void create_other_players(std::vector<PlayerCreationRequest> other_players);

	void create_player(PlayerCreationRequest player);

protected:

	void Init() override;

	void Update(float dt) override;

private:

	static NetPlayerManager* m_instance;

	LocalPlayerCharacter* m_local_player;
	uint32_t m_local_player_id;

	double m_last_sent_location{ 0.0 };
	int m_frame_counter{ 0 };

	std::unordered_map<uint8_t, NetPlayerCharacter*> m_net_player_map_InstID;
	std::unordered_map<uint32_t, NetPlayerCharacter*> m_net_player_map_ID;

	void send_player_location();


	void add_net_commands();

	inline static const std::string LOG_LOC{ "NET_PLAYER_MANAGER" };

};