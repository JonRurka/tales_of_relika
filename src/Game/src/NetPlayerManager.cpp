#include "NetPlayerManager.h"

#include "NetPlayerCharacter.h"
#include "LocalPlayerCharacter.h"
#include "GameClient.h"
#include "Network/NetClient.h"
#include "Network/BufferUtils.h"
#include "Player.h"

#include "glaze/glaze.hpp" 

#define ORIENTATION_SEND_RATE ((1 / 20.0) * 1.0) // MS

NetPlayerManager* NetPlayerManager::m_instance{nullptr};

void NetPlayerManager::RegisterLocalPlayer(LocalPlayerCharacter* local_player)
{
	m_local_player = local_player;
}

void NetPlayerManager::OnUpdateOrientations(Data data)
{
	uint8_t num_orientations = data.Buffer[0];
	data.Buffer = BufferUtils::RemoveFront(1, data.Buffer);

	const int orientation_size = 1 + sizeof(float) * 7;
	for (int i = 0; i < num_orientations; i++) {

		uint8_t* packet = &data.Buffer.data()[i * orientation_size];

		uint8_t player_inst_id = packet[0];

		float* orientation_buff = (float*)(&packet[1]);

		glm::vec3 player_loc = glm::vec3(orientation_buff[0] * 100, orientation_buff[1] * 100, orientation_buff[2] * 100);
		glm::quat player_rot = glm::quat(orientation_buff[3], orientation_buff[4], orientation_buff[5], orientation_buff[6]);

		if (m_net_player_map_InstID.contains(player_inst_id)) {
			//UE_LOG(NetPlayerManager_Log, Display, TEXT("Received location update 1"));
			m_net_player_map_InstID[player_inst_id]->Set_Orientation(player_loc, player_rot);
		}

	}


}

void NetPlayerManager::OnPlayerEvent(Data data)
{
	uint8_t num_updates = data.Buffer[0];
	data.Buffer = BufferUtils::RemoveFront(1, data.Buffer);

	for (int i = 0, index = 0; i < num_updates; i++) {

		uint8_t* packet = &data.Buffer.data()[index];

		uint8_t player_inst_id = packet[0];
		uint8_t event_cmd = packet[1];
		uint8_t data_size = packet[2];

		std::vector<uint8_t> event_data;
		if (data_size > 0) {
			event_data = std::vector<uint8_t>(&packet[3], &packet[3] + data_size);
		}

		index += data_size + 3;

		if (m_net_player_map_InstID.contains(player_inst_id)) {
			m_net_player_map_InstID[player_inst_id]->Execute_Player_Event((OpCodes::Player_Events)event_cmd, event_data);
		}
	}

}

void NetPlayerManager::RequestPlayers()
{
	std::vector<uint8_t> data;
	data = BufferUtils::AddFirst((uint8_t)OpCodes::Server_World::Request_Players, data);
	GameClient::Instance()->Net_Client()->Send(OpCodes::Server::World_Command, data, Protocal_Udp);
}

void NetPlayerManager::SpawnPlayers(Data data)
{
	std::string json_str = data.Input;

	std::vector<PlayerCreationRequest> player_requests;
	json player_data_arr = json::parse(json_str);
	player_requests.reserve(player_data_arr.size());
	for (int i = 0; i < player_data_arr.size(); i++) 
	{
		NetPlayerManager::PlayerCreationRequest req;
		req.UserName = player_data_arr[i]["UserName"];
		req.User_ID = player_data_arr[i]["User_ID"];
		req.Instance_ID = player_data_arr[i]["Instance_ID"];

		json pos_obj = player_data_arr[i]["Position"];
		req.Position = glm::vec3(pos_obj["X"], pos_obj["Y"], pos_obj["Z"]);

		json rot_obj = player_data_arr[i]["Rotation"];
		req.Rotation = glm::quat(rot_obj["X"], rot_obj["Y"], rot_obj["Z"], rot_obj["W"]);

		player_requests.push_back(req);
	}

	/*auto s = glz::read_json<std::vector<Player::PlayerSpawnData>>(json_str);
	if (!s)
	{
		return;
	}*/
	/*std::vector<Player::PlayerSpawnEntry> player_arr;// = s.value();
	std::vector<PlayerCreationRequest> player_requests;
	player_requests.reserve(player_arr.size());
	for (auto& player_data : player_arr)
	{
		NetPlayerManager::PlayerCreationRequest req;
		req.UserName = player_data.UserName;
		req.User_ID = player_data.User_ID;
		req.Instance_ID = player_data.Instance_ID;
		req.Position = player_data.Position.glm_vec();
		req.Rotation = player_data.Rotation.glm_quat();
		player_requests.push_back(req);
	}*/

	create_other_players(player_requests);
}

void NetPlayerManager::create_other_players(std::vector<PlayerCreationRequest> other_players)
{
	for (int i = 0; i < other_players.size(); i++) {
		create_player(other_players[i]);
	}

}

void NetPlayerManager::create_player(PlayerCreationRequest player)
{
	if (player.User_ID == m_local_player_id) {
		return; // Don't create local player.
	}

	WorldObject* obj = Instantiate(player.UserName);
	NetPlayerCharacter* character = obj->Add_Component<NetPlayerCharacter>();
	character->Init(player.UserName, player.User_ID, player.Position, player.Rotation);

	if (!m_net_player_map_InstID.contains(player.Instance_ID) &&
		m_net_player_map_ID.contains(player.User_ID)) {
		Logger::Log(LOG_POS("CreatePlayer"), "Added net player '%s'", player.UserName.c_str());
		m_net_player_map_InstID[player.Instance_ID] = character;
		m_net_player_map_ID[player.User_ID] = character;
	}
}

void NetPlayerManager::Init()
{
	m_instance = this;
	m_last_sent_location = Utilities::Get_Time();
	add_net_commands();
	RequestPlayers();
}

void NetPlayerManager::Update(float dt)
{
	double now = Utilities::Get_Time();

	if ((now - m_last_sent_location) > ORIENTATION_SEND_RATE) {
		send_player_location();
		m_last_sent_location = Utilities::Get_Time();
	}

	m_frame_counter++;
}

void NetPlayerManager::send_player_location()
{
	if (LocalPlayerCharacter::Get_Instance() == nullptr) {
		//UE_LOG(NetPlayerManager_Log, Display, TEXT("m_local_player null"));
		return;
	}

	if (GameClient::Instance()->Net_Client() == nullptr ||
		!GameClient::Instance()->Net_Client()->Connected())
	{
		//UE_LOG(NetPlayerManager_Log, Display, TEXT("Net Client not connected"));
		return;
	}


	m_local_player = LocalPlayerCharacter::Get_Instance();

	glm::vec3 location = m_local_player->Object()->Get_Transform()->Position();
	glm::quat rotation = m_local_player->Object()->Get_Transform()->Rotation();

	float or_buf[7] = {
		location.x,
		location.y,
		location.z,
		rotation.x,
		rotation.y,
		rotation.z,
		rotation.w
	};

	std::vector<uint8_t> data = std::vector<uint8_t>((uint8_t*)(or_buf), (uint8_t*)(or_buf + sizeof(or_buf)));
	data = BufferUtils::AddFirst((uint8_t)OpCodes::Server_World::Update_Orientation, data);

	GameClient::Instance()->Net_Client()->Send(OpCodes::Server::World_Command, data, Protocal_Udp);
}

void NetPlayerManager::add_net_commands()
{
	GameClient::Instance()->Net_Client()->AddCommand(OpCodes::Client::Update_Orientations, NetPlayerManager::OnUpdateOrientations_cb, this);
	GameClient::Instance()->Net_Client()->AddCommand(OpCodes::Client::Player_Events, NetPlayerManager::OnPlayerEvent_cb, this);
}
