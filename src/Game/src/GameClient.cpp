#include "GameClient.h"

#include <nlohmann/json.hpp>

#include "Network/NetClient.h"
#include "Network/OpCodes.h"
#include "Network/BufferUtils.h"


#define HOST ""

GameClient* GameClient::m_instance{nullptr};


void GameClient::Init(std::string user_name, int32_t id, bool remote)
{
	m_user_ID = id;
	m_debug_userName = user_name;

	NetClient::HostType type = remote ? NetClient::HostType::Remote : NetClient::HostType::Local;

	m_client = new NetClient("game_client", type, HOST);
	m_client->SetOnConnectSuccess(GameClient::OnConnect, this);
	m_client->AddCommand(OpCodes::Client::Identify_Result, GameClient::OnIdentifyResult_cb, this);
}

void GameClient::Connect()
{
	m_client->Connect();
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
		//do_identify();

	}
	else {
		Logger::Log(LOG_POS("ClientConnected"), "We're NOT connected!");
	}
}

void GameClient::OnIdentifyResult(Data data)
{



}

void GameClient::do_identify()
{
	json ident_json;

	ident_json["UserName"] = "";
	ident_json["UserID"] = 0;
	
	std::string ident_jsn_str = ident_json.dump();

}
