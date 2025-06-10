#include "World.h"
#include "Server_Main.h"
#include "Network/AsyncServer.h"
#include "Network/SocketUser.h"
#include "IUser.h"
#include "Player.h"
#include "HashHelper.h"
#include "LuaEngine.h"
#include "WorldController.h"

#define ORIENTATION_SEND_RATE ((1 / 20.0) * 1000) // MS

					  
void World::Init()
{
	m_thread = std::thread(Run, this);
}

void World::Stop(bool trigger_events)
{


}

void World::Update(float dt)
{
}

void World::SubmitPlayerEvent(Player& player, OpCodes::Player_Events event_cmd, std::vector<uint8_t> data)
{
	if (player.Socket_User()->Get_Authenticated()) {
		player.Add_Player_Event(event_cmd, data);
	}
}

World* World::New_World(WorldCreationOptions options)
{
	World* world = new World();
	world->create_new(options);
	world->Init();
	return world;
}

World* World::Load_World(uint64_t world_id)
{
	World* world = new World();
	world->load(world_id);
	world->Init();
	return world;
}

void World::AssignPlayer(World* world, Player::pointer player)
{
	World* current_world = player->Get_Current_World();

	if (current_world != nullptr) {

		if (world != nullptr) {
			if (current_world->World_ID() == world->World_ID()) {
				return;
			}
		}
		current_world->remove_player(player);
	}

	if (world != nullptr) {
		world->add_player(player);
	}
}

void World::SubmitWorldCommand(Player* user, Data data)
{
	NetCommand command{};
	command.user = user->Get_UserID();
	command.data = data;

	m_command_queue_lock.lock();
	m_command_queue.push(command);
	m_command_queue_lock.unlock();
}

bool World::HasPlayer(uint32_t player_id)
{
	return m_players.contains(player_id);
}

bool World::HasPlayer(Player::pointer player)
{
	return m_players.contains(player->Get_UserID());
}

std::vector<Player::pointer> World::GetPlayers()
{
	std::vector<Player::pointer> current_players;
	current_players.reserve(m_players.size());
	m_world_mtx.lock();
	for (auto& pair : m_players) {
		current_players.push_back(pair.second);
	}
	m_world_mtx.unlock();

	return std::vector<Player::pointer>();
}

std::vector<Player::pointer> World::PlayersInRadius(glm::vec3 point, float radius)
{
	std::vector<Player::pointer> players = GetPlayers();
	std::vector<Player::pointer> close_players;
	close_players.reserve(players.size());
	for (auto& p : players) {
		glm::vec3 player_pos = p->Get_Location();
		float dist = glm::distance(point, player_pos);
		if (dist <= radius) {
			close_players.push_back(p);
		}
	}
	return close_players;
}

void World::WorldMutexLock()
{
	m_world_mtx.lock();
}

void World::WorldMutexUnlock()
{
	m_world_mtx.unlock();
}

void World::Run(World* world)
{
	world->async_init();
	world->GameLoop();
}

void World::Register_Lua_Functions(sol::state lua)
{
	


}

void World::async_init()
{
	m_running = true;
	m_last_orientation_update = Server_Main::GetEpoch();
	m_last_frame = Server_Main::GetEpoch();
	async_init();
}

void World::init_lua()
{
	sol::state lua = Server_Main::GetLuaEngine()->NewServerState();
	std::string match_script = Server_Main::GetLuaEngine()->Get_Script("Match");

	lua.script(match_script);

	init_lua_script(lua);

	TRIGGER_EVENT(m_init_events);
}

void World::init_lua_script(sol::state& lua)
{
	// load events
	ADD_LUA_EVENT(lua, "init", m_init_events);
	ADD_LUA_EVENT(lua, "update", m_update_events);
	ADD_LUA_EVENT(lua, "player_added", m_player_add_events);
	ADD_LUA_EVENT(lua, "player_removed", m_player_remove_events);

	// load variables
	lua["World_ID"] = m_world_id;

	
}

bool World::add_player(Player::pointer player, bool trigger_events)
{
	uint32_t user_id = player->Get_UserID();
	if (m_players.contains(user_id)) {
		return true;
	}

	bool should_add = true;
	if (trigger_events) {
		for (const auto& func : m_player_add_events) {
			should_add |= func(user_id);
		}
	}

	if (!should_add)
		return false;

	uint16_t inst_id = 0;
	bool contains_id = false;
	int i = 0;
	do {
		inst_id = HashHelper::RandomNumber(0, UINT16_MAX - 1);
		contains_id = m_player_short_ids.contains(inst_id);
		i++;
	} while (contains_id);

	player->Set_Current_World(this, inst_id);
	m_players[inst_id] = player;
	m_player_short_ids[inst_id] = user_id;

	return true;
}

bool World::remove_player(Player::pointer player, bool trigger_events)
{
	uint32_t user_id = player->Get_UserID();
	if (!m_players.contains(user_id)) {
		return true;
	}

	bool should_remove = true;
	if (trigger_events) {
		for (const auto& func : m_player_remove_events) {
			should_remove |= func(user_id);
		}
	}

	if (!should_remove)
		return false;

	uint16_t inst_id = player->Get_WorldInstanceID();
	player->Set_Current_World(nullptr, 0);
	m_players.erase(user_id);
	m_player_short_ids.erase(inst_id);

	return true;
}

void World::create_new(WorldCreationOptions options)
{
	m_world_id = HashHelper::RandomNumber_u64(0, UINT64_MAX - 1);
}

void World::load(uint64_t id)
{
	m_world_id = id;

}

void World::GameLoop()
{
	while (m_running) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		uint64_t now = Server_Main::GetEpoch();
		float delta_time = (now - m_last_frame) / 1000.0;
		m_last_frame = now;

		AsynUpdate(delta_time);
	}
}

void World::AsynUpdate(float dt)
{
	ProcessNetCommands();
	UpdatePlayers(dt);
	SendOrientationUpdates();
	SendPlayerEvents();
}

void World::UpdatePlayers(float dt)
{
	std::vector<Player::pointer> current_players = GetPlayers();
	for (auto& p : current_players) {
		p->WorldUpdate(dt);
	}
}

void World::SendOrientationUpdates()
{
	uint64_t now = Server_Main::GetEpoch();

	if ((now - m_last_orientation_update) > ORIENTATION_SEND_RATE) {

		std::vector<Player::pointer> current_players = GetPlayers();

		for (const auto& p : current_players) {
			p->SyncOrientations();
		}
	}
}

void World::SendPlayerEvents()
{
	
}

void World::ProcessNetCommands()
{
	std::queue<NetCommand> current_commands;
	threadSafeCommandQueueDuplicate(m_command_queue_lock, m_command_queue, current_commands);

	while (!current_commands.empty()) {
		NetCommand data = current_commands.front();
		current_commands.pop();

		ExecuteNetCommand(data.user, data.data);
	}
}

int World::threadSafeCommandQueueDuplicate(std::mutex& lock, std::queue<NetCommand>& from, std::queue<NetCommand>& to)
{
	int res = 0;
	lock.lock();
	while (!from.empty()) {
		to.push(from.front());
		from.pop();
		res++;
	}
	lock.unlock();
	return res;
}

void World::ExecuteNetCommand(uint32_t user, Data data)
{
	if (!HasPlayer(user))
		return;

	Player* player = m_players[user].get();

	if (data.Buffer.size() > 0) 
	{
		OpCodes::Server_World sub_command = (OpCodes::Server_World)data.Buffer[0];
		data.Buffer = BufferUtils::RemoveFront(Remove_CMD, data.Buffer);

		switch (sub_command) {
		case OpCodes::Server_World::Update_Orientation:
			UpdateOrientation_NetCmd(*player, data);
			break;
		case OpCodes::Server_World::Player_Event:
			if (data.Buffer.size() > 0)
			{
				OpCodes::Player_Events event_cmd = (OpCodes::Player_Events)data.Buffer[0];
				data.Buffer = BufferUtils::RemoveFront(Remove_CMD, data.Buffer);
				SubmitPlayerEvent(*player, event_cmd, data.Buffer);
			}
			else {
				Logger::LogWarning(LOG_POS("ExecuteNetCommand"), "Received malformed Match Player Event from '" + player->Get_UserName() + "'!");
			}
			break;
		}
	}
}

void World::UpdateOrientation_NetCmd(Player& player, Data data)
{
	float* loc_buff = (float*)data.Buffer.data();
	//float* rot_buff = &((float*)data.Buffer.data())[3];

	float loc_x = loc_buff[0];
	float loc_y = loc_buff[1];
	float loc_z = loc_buff[2];

	float rot_x = loc_buff[3];
	float rot_y = loc_buff[4];
	float rot_z = loc_buff[5];
	float rot_w = loc_buff[6];

	glm::vec3 location = glm::vec3(loc_x, loc_y, loc_z);
	glm::quat rotation = glm::quat(rot_w, rot_x, rot_y, rot_z);

	player.Set_Location(location);
	player.Set_Rotation(rotation);

	std::string loc_str = "(" + std::to_string(loc_x) + ", " + std::to_string(loc_y) + ", " + std::to_string(loc_z) + ")";
	std::string rot_str = "(" + std::to_string(rot_x) + ", " + std::to_string(rot_y) + ", " + std::to_string(rot_z) + ", " + std::to_string(rot_w) + ")";

	//Logger::Log("Received orientation update ("+std::to_string(data.Buffer.size()) + "): " + loc_str + ", " + rot_str + ", " + std::to_string(data.Type));
}




void World::LuaBridge::Stop(uint64_t world_id)
{
	World* world = WorldController::GetInstance()->Get_World(world_id);
	if (!world) {
		Logger::LogError(LOG_POS("LUA::Stop"), "World not found!");
		return;
	}
	world->WorldMutexLock();
	world->Stop(false);
	world->WorldMutexUnlock();
}

bool World::LuaBridge::HasPlayer(uint64_t world_id, uint32_t player_id)
{
	World* world = WorldController::GetInstance()->Get_World(world_id);
	if (!world) {
		Logger::LogError(LOG_POS("LUA::HasPlayer"), "World not found!");
		return false;
	}

	world->WorldMutexLock();
	bool has_player = world->HasPlayer(player_id);
	world->WorldMutexUnlock();

	return has_player;
}

std::vector<uint32_t> World::LuaBridge::GetPlayers(uint64_t world_id)
{
	std::vector<uint32_t> res;
	World* world = WorldController::GetInstance()->Get_World(world_id);
	if (!world) {
		Logger::LogError(LOG_POS("LUA::GetPlayers"), "World not found!");
		return res;
	}

	world->WorldMutexLock();
	std::vector<Player::pointer> players = world->GetPlayers();
	for (const auto& p : players) {
		res.push_back(p->Get_UserID());
	}
	world->WorldMutexUnlock();

	return res;
}

std::vector<uint32_t> World::LuaBridge::PlayersInRadius(uint64_t world_id, float x, float y, float z, float radius)
{
	std::vector<uint32_t> res;
	World* world = WorldController::GetInstance()->Get_World(world_id);
	if (!world) {
		Logger::LogError(LOG_POS("LUA::PlayersInRadius"), "World not found!");
		return res;
	}

	world->WorldMutexLock();
	std::vector<Player::pointer> players = world->PlayersInRadius(glm::vec3(x, y, z), radius);
	for (const auto& p : players) {
		res.push_back(p->Get_UserID());
	}
	world->WorldMutexUnlock();

	return res;
}
