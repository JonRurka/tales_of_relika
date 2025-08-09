#include "Client_Server_Interface.h"

#include "Logger.h"
#include "Server_Main.h"

void Client_Server::Initialize_Server()
{
	m_server->Start();
}

void Client_Server::Init()
{
	Server_Main::Options options;
	options.Type = Server_Main::Server_Type::Local;
	options.Async = true;
	m_server = std::make_unique<Server_Main>(options);
}

void Client_Server::Update(float dt)
{


}

void Client_Server::OnDestroy()
{
	m_server->Dispose();
	m_server.reset();
}
