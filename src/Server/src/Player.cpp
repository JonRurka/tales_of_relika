#include "Player.h"
#include "Server_Main.h"
#include "World.h"
#include "WorldController.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#define SEND_DEBUG_JUMP 3000
#define PLAYER_SCAN_TIME 5000
#define PLAYER_SCAN_RADIUS 100

Player::Player()
{
	m_sent_last_jump = Server_Main::GetEpoch();
	m_world_instance_id = 0;
	m_location = glm::vec3(0, 0, 0);
	m_rotation = glm::quat();
	m_sent_last_jump = Server_Main::GetEpoch();

}

Player::~Player()
{
}

std::shared_ptr<Player> Player::Cast_IUser(std::weak_ptr<IUser> user)
{
	return std::dynamic_pointer_cast<Player>(user.lock());
}

bool Player::SetIdentity(std::string json_identity)
{
	json data = json::parse(json_identity);

	m_identity.UserName = data["UserName"];
	m_identity.UserID = data["UserID"];

	//Logger::Log( json_identity);
	/*
	json::parse_options opt;
	opt.allow_trailing_commas = true;

	json::error_code ec;
	json::value json_val = json::parse(json_identity, ec, json::storage_ptr(), opt);

	if (ec) {
		//Logger::Log("Failed to parse user identity.");
		return false;
	}

	json::object ident_obj = json_val.as_object();

	m_identity.UserName = std::string(ident_obj.at("UserName").as_string());
	m_identity.User_Distro_ID = std::string(ident_obj.at("User_Distro_ID").as_string());
	m_identity.UserID = ident_obj.at("UserID").as_int64();

	//Logger::Log("Is Number: " + std::to_string((int)ident_obj.at("Distributor").is_number()) + ", " + std::to_string((int)ident_obj.at("Distributor").is_int64()));
	m_identity.Distributor = ident_obj.at("Distributor").as_int64();
	*/
	return true;
}

bool Player::SetIdentity(PlayerIdentity identity)
{
	m_identity = identity;

	return true;
}

void Player::WorldUpdate(float dt)
{
	

	if (m_identity.UserID == 0) {


		auto now = Server_Main::GetEpoch();

		if ((now - m_sent_last_jump) > SEND_DEBUG_JUMP) {

			m_sent_last_jump = Server_Main::GetEpoch();
		}

		if ((now - m_last_player_scan) > PLAYER_SCAN_TIME) {
			update_nearby_players();
			m_last_player_scan = Server_Main::GetEpoch();
		}


		//m_location += glm::vec3(6 * dt, 0, 0);


		//Add_Player_Event(OpCodes::Player_Events::Jump);
	}

	if (m_trigger_save) {
		save_player_data();
		m_trigger_save = false;
	}

}

void Player::AssignPlayer(World* world)
{
	World::AssignPlayer(world, Server_Main::GetPlayer(m_identity.UserID));
}

bool Player::LoadPlayerData()
{




	return false;
}

void Player::SyncOrientations()
{
	uint8_t num_orientations = m_nearby_players.size();
	int player_entry_size = (OrientationSize() + 1);
	
	int m_orientation_send_buffer_size = (player_entry_size * num_orientations) + 1;
	uint8_t* m_orientation_send_buffer = new uint8_t[m_orientation_send_buffer_size];
	
	m_orientation_send_buffer[0] = num_orientations;

	int p_index = 0;
	for (const auto& p : m_nearby_players)
	{
		int buffer_index = (p_index * player_entry_size) + 1;
		m_orientation_send_buffer[buffer_index] = Get_WorldInstanceID();
		p->Serialize_Orientation(&m_orientation_send_buffer[buffer_index + 1]);
		p_index++;
	}

	std::vector<uint8_t> send_buff(m_orientation_send_buffer, m_orientation_send_buffer + m_orientation_send_buffer_size);
	Send(OpCodes::Client::Update_Orientations, send_buff, Protocal_Udp); 

}

void Player::PlayerMutexLock()
{
	m_player_mutex.lock();
}

void Player::PlayerMutexUnlock()
{
	m_player_mutex.unlock();
}

void Player::update_nearby_players()
{
	if (m_current_world == nullptr) {
		return;
	}

	m_nearby_players = m_current_world->PlayersInRadius(m_location, PLAYER_SCAN_RADIUS);
}


void Player::save_player_data()
{
	m_game_data.CurrentWorldID = m_current_world->World_ID();

	// Save data

}


void Player::Register_Lua_Functions(sol::state lua)
{
	const std::string name = "Players";
	lua[name] = lua.create_table();
	lua[name]["AssignPlayer"] = &Player::LuaBridge::AssignPlayer;
	lua[name]["GetCurrentWorld"] = &Player::LuaBridge::Get_Current_World;
	lua[name]["SetLocation"] = &Player::LuaBridge::Set_Location;
	lua[name]["GetLocation"] = &Player::LuaBridge::Get_Location;
	lua[name]["SetRotation"] = &Player::LuaBridge::Set_Rotation;
	lua[name]["GetRotation"] = &Player::LuaBridge::Get_Rotation;
}

void Player::LuaBridge::AssignPlayer(int player_handle, int world_handle)
{
	Player::pointer player = Server_Main::GetPlayer(player_handle);
	World* world = WorldController::GetInstance()->Get_World(world_handle);

	if (!world){
		Logger::LogError(LOG_POS("LUA::Stop"), "World not found!");
		return;
	}

	world->WorldMutexLock();
	player->AssignPlayer(world);
	world->WorldMutexUnlock();
}

uint64_t Player::LuaBridge::Get_Current_World(int player_handle)
{
	Player::pointer player = Server_Main::GetPlayer(player_handle);

	return 0;
}

void Player::LuaBridge::Set_Location(int player_handle, float x, float y, float z)
{
	Player::pointer player = Server_Main::GetPlayer(player_handle);

	player->PlayerMutexLock();
	player->Set_Location(glm::vec3(x, y, z));
	player->PlayerMutexUnlock();
}

glm::fvec3 Player::LuaBridge::Get_Location(int player_handle)
{
	Player::pointer player = Server_Main::GetPlayer(player_handle);

	glm::fvec3 res;
	player->PlayerMutexLock();
	res = player->Get_Location();
	player->PlayerMutexUnlock();

	return res;
}

void Player::LuaBridge::Set_Rotation(int player_handle, glm::quat rot)
{
	Player::pointer player = Server_Main::GetPlayer(player_handle);

	player->PlayerMutexLock();
	player->Set_Rotation(rot);
	player->PlayerMutexUnlock();
}

glm::quat Player::LuaBridge::Get_Rotation(int player_handle)
{
	Player::pointer player = Server_Main::GetPlayer(player_handle);

	glm::quat res;
	player->PlayerMutexLock();
	res = player->Get_Rotation();
	player->PlayerMutexUnlock();

	return res;
}
