#pragma once

#include "stdafx.h"
#include "Network/Data.h"
#include "Network/OpCodes.h"
#include "Player.h"

#include <thread>

class SocketUser;
class Player;

class World {
public:

	struct WorldCreationOptions {

	};

	void Init();

	void Stop();

	void Update(float dt);

	void SubmitPlayerEvent(Player& user, OpCodes::Player_Events, std::vector<uint8_t> data);

	static World* New_World(WorldCreationOptions options);

	static World* Load_World(uint64_t world_id);

	static void AssignPlayer(World* world, Player::pointer player);

	uint64_t World_ID() { return m_world_id; }

	void SubmitWorldCommand(Player* user, Data data);

	bool HasPlayer(uint32_t player_id);

	bool HasPlayer(Player::pointer player);

	std::vector<Player::pointer> GetPlayers();

	std::vector<Player::pointer> PlayersInRadius(glm::vec3 point, float radius);

	static void Run(World* world);

private:

	struct NetCommand {
	public:
		uint32_t user;
		Data data;
	};

	std::thread m_thread;
	std::mutex m_player_mtx;

	uint64_t m_world_id{ 0 };

	std::queue<NetCommand> m_command_queue;
	std::mutex m_command_queue_lock;

	std::map<uint32_t, std::shared_ptr<Player>> m_players;
	std::map<uint16_t, uint32_t> m_player_short_ids;

	bool m_running{ false };
	uint64_t m_last_orientation_update{ 0 };
	uint64_t m_last_frame{ 0 };

	uint8_t* m_orientation_send_buffer = nullptr;
	int m_orientation_send_buffer_size = 0;

	void async_init();

	void add_player(Player::pointer player);

	void remove_player(Player::pointer player);

	void create_new(WorldCreationOptions options);

	void load(uint64_t id);

	void GameLoop();

	void AsynUpdate(float dt);

	void UpdatePlayers(float dt);

	void SendOrientationUpdates();

	void SendPlayerEvents();

	void ProcessNetCommands();

	int threadSafeCommandQueueDuplicate(std::mutex& lock, std::queue<NetCommand>& from, std::queue<NetCommand>& to);

	void ExecuteNetCommand(uint32_t user, Data data);

	void UpdateOrientation_NetCmd(Player& user, Data data);


	inline static const std::string LOG_LOC{ "WORLD" };
};