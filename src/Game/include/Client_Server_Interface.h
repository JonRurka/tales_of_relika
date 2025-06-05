#pragma once

#include "game_engine.h"

class Server_Main;

class Client_Server : public Component {
public:

	void Initialize_Server();

protected:
	void Init() override;

	void Update(float dt) override;


private:

	Server_Main* m_server{nullptr};


};