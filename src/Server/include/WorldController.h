#pragma once

#include "stdafx.h"
#include "Network/Data.h"

class SocketUser;
class Player;

class WorldController {
public:

	WorldController();
	~WorldController();

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

private:

	static WorldController* m_instance;


	inline static const std::string LOG_LOC{ "WORLD_CONTROLLER" };

};