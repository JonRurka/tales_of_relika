#pragma once

#include <stdint.h>

#include "game_engine.h"

#include "Network/Data.h"


class NetClient;

class GameClient : public Component {
public:
	
	typedef void(*OnGameConnectActionPtr)(void*);

	static GameClient* Instance() {
		return m_instance;
	}

	static NetClient* Net_Client() {
		return m_instance->m_client;
	}

	void Init(std::string user_name, int32_t id, bool remote);

	void SetOnConnectSuccess(OnGameConnectActionPtr cb, void* p) {
		OnGameConnect_delegate = cb;
		OnGameConnect_obj = p;
	}

	void Connect();

protected:
	void Init() override;

	void Update(float dt) override;

	static void OnConnect(void* obj, bool success);
	void ClientConnected(bool success);

	static void OnIdentifyResult_cb(void* obj, Data data) {
		m_instance->OnIdentifyResult(data);
	}
	void OnIdentifyResult(Data data);

private:

	NetClient* m_client{ nullptr };
	static GameClient* m_instance;

	OnGameConnectActionPtr OnGameConnect_delegate;
	void* OnGameConnect_obj{ nullptr };

	int32_t m_local_player_user_id;

	std::string m_debug_userName;
	int m_user_ID{ 0 };

	void do_identify();

	inline static const std::string LOG_LOC{ "GAME_CLIENT" };
};