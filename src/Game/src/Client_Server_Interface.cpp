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
	m_server = new Server_Main(options);
}

void Client_Server::Update(float dt)
{


}
