#pragma once

#include "game_engine.h"

#include "Server_Main.h"

#include <memory>

class Server_Main;

class Client_Server : public Component {
public:
	typedef std::shared_ptr<Client_Server> Shared;
	typedef std::weak_ptr<Client_Server> Weak;

	void Initialize_Server();

protected:
	void Init() override;

	void Update(float dt) override;

	void OnDestroy() override;


private:

	std::unique_ptr<Server_Main> m_server;


};