#pragma once

#include "stdafx.h"
#include "Network/Data.h"

class SocketUser;
class Player;

class WorldController {
public:

	WorldController();
	~WorldController();

	void Init();

	void Update(float dt);

	static WorldController* GetInstance() {
		return m_instance;
	}

	static void RoutWorldNetCommand_cb(void* obj, SocketUser& user, Data data) {
		m_instance->RoutWorldNetCommand(user, data);
	}
	void RoutWorldNetCommand(SocketUser& user, Data data);

	void Create_World();

	void Load_Worlds();

	bool World_Exists(uint64_t world_id);

	World* Get_World(uint64_t world_id);

	World* Default_World() { return m_default_world; }

private:

	static WorldController* m_instance;

	std::unordered_map<uint64_t, World*> m_loaded_worlds;
	World* m_default_world{ nullptr };


	inline static const std::string LOG_LOC{ "WORLD_CONTROLLER" };

};