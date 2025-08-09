#include "GameClient.h"

#include <nlohmann/json.hpp>

#include "Network/NetClient.h"
#include "Network/OpCodes.h"
#include "Network/BufferUtils.h"


#define HOST ""

GameClient* GameClient::m_instance{nullptr};


void GameClient::Init(std::string user_name, std::string host, int32_t id, bool remote)
{
	m_user_ID = id;
	m_debug_userName = user_name;

	NetClient::HostType type = remote ? NetClient::HostType::Remote : NetClient::HostType::Local;

	if (type == NetClient::HostType::Local)
	{
		host = HOST;
	}

	m_client = std::make_unique<NetClient>("game_client", type, host);
	m_client->SetOnConnectSuccess(GameClient::OnConnect, this);
	m_client->AddCommand(OpCodes::Client::Identify_Result, GameClient::OnIdentifyResult_cb, this);
	m_has_client = true;
}

void GameClient::Connect()
{
	m_client->Connect();
}

void GameClient::Send_World(OpCodes::Server_World cmd)
{
	Send_World(cmd, std::vector<uint8_t>());
}

void GameClient::Send_World(OpCodes::Server_World cmd, std::vector<uint8_t> data)
{
	std::vector<uint8_t> send_data;
	send_data.push_back((uint8_t)cmd);
	send_data = BufferUtils::Add(send_data, data);
	m_client->Send(OpCodes::Server::World_Command, send_data);
}

void GameClient::Init()
{
	m_instance = this;
}

void GameClient::Update(float dt)
{
	m_client->Process();
}

void GameClient::OnConnect(void* obj, bool success)
{
	GameClient* game_client = (GameClient*)obj;
	game_client->ClientConnected(success);
}

void GameClient::ClientConnected(bool success)
{
	if (success) {
		Logger::Log(LOG_POS("ClientConnected"), "We're connected!");
		do_identify();

	}
	else {
		Logger::Log(LOG_POS("ClientConnected"), "We're NOT connected!");
	}
}

void GameClient::OnIdentifyResult(Data data)
{
	//Logger::LogDebug(LOG_POS("OnIdentifyResult"), "Received an identify result.");

	OnGameConnect_delegate(OnGameConnect_obj);
}

void GameClient::do_identify()
{
	json ident_json;

	ident_json["UserName"] = m_debug_userName;
	ident_json["UserID"] = m_user_ID;
	
	std::string ident_jsn_str = ident_json.dump();

	m_client->Send(OpCodes::Server::Submit_Identity, ident_jsn_str);
}
