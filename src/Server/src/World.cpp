#include "World.h"

#include "Network/AsyncServer.h"
#include "Network/SocketUser.h"
#include "IUser.h"
#include "Player.h"
#include "HashHelper.h"

void World::Init()
{
}

void World::Stop()
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

World* World::New_World()
{
	World* world = new World();
	world->create_new();
	return world;
}

World* World::Load_World(uint64_t world_id)
{
	World* world = new World();
	world->load(world_id);
	return world;
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

bool World::HasPlayer(std::shared_ptr<Player> player)
{
	return m_players.contains(player->Get_UserID());
}

void World::create_new()
{
	m_world_id = HashHelper::RandomNumber_u64(0, UINT64_MAX - 1);
}

void World::load(uint64_t id)
{
	m_world_id = id;

}

void World::GameLoop()
{
}

void World::AsynUpdate(float dt)
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
				Logger::LogWarning("Received malformed Match Player Event from '" + player->Get_UserName() + "'!");
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
