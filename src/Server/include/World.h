#pragma once

#include "stdafx.h"
#include "Network/Data.h"
#include "Network/OpCodes.h"

class SocketUser;
class Player;

class World {
public:

	void Init();

	void Stop();

	void Update(float dt);

	void SubmitPlayerEvent(Player& user, OpCodes::Player_Events, std::vector<uint8_t> data);

	static World* New_World();

	static World* Load_World(uint64_t world_id);

	uint64_t World_ID() { return m_world_id; }

	void SubmitWorldCommand(Player* user, Data data);

	bool HasPlayer(uint32_t player_id);

	bool HasPlayer(std::shared_ptr<Player> player);

private:

	struct NetCommand {
	public:
		uint32_t user;
		Data data;
	};

	uint64_t m_world_id{ 0 };

	std::queue<NetCommand> m_command_queue;
	std::mutex m_command_queue_lock;

	std::map<uint32_t, std::shared_ptr<Player>> m_players;

	void create_new();

	void load(uint64_t id);

	void GameLoop();

	void AsynUpdate(float dt);

	void ProcessNetCommands();

	int threadSafeCommandQueueDuplicate(std::mutex& lock, std::queue<NetCommand>& from, std::queue<NetCommand>& to);

	void ExecuteNetCommand(uint32_t user, Data data);

	void UpdateOrientation_NetCmd(Player& user, Data data);

};