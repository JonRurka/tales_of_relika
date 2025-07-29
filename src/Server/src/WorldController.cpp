#include "WorldController.h"

#include "HashHelper.h"
#include "Network/BufferUtils.h"
#include "Network/AsyncServer.h"
#include "Network/SocketUser.h"
#include "Network/OpCodes.h"
#include "IUser.h"
#include "Player.h"
#include "World.h"

WorldController* WorldController::m_instance{nullptr};

WorldController::WorldController()
{
	m_instance = this;
	AsyncServer::GetInstance()->AddCommand(OpCodes::Server::World_Command, WorldController::RoutWorldNetCommand_cb, this, true);
	//Logger::LogDebug(LOG_POS("NEW"), "Added commands");
}

WorldController::~WorldController()
{
}

void WorldController::Init()
{
	Create_World();
}

void WorldController::Update(float dt)
{
}

void WorldController::RoutWorldNetCommand(SocketUser& user, Data data)
{
	if (!user.Get_Authenticated()) {
		// send fail
		Logger::Log(LOG_POS("RoutWorldNetCommand"), "World command route failed: user not authenticated!");
		return;
	}


	Player* player = Player::Cast_IUser(user.GetUser()).get();

	if (player != nullptr && player->Get_Current_World() != nullptr) {
		World* curr_world = player->Get_Current_World();
		curr_world->SubmitWorldCommand(player, data);
	}

}

void WorldController::Create_World()
{
	World::WorldCreationOptions options{};
	World* world = World::New_World(options);

	uint64_t world_id = world->World_ID();
	m_loaded_worlds[world_id] = world;
	m_default_world = world;
}

void WorldController::Load_Worlds()
{
	World::WorldCreationOptions options{};
	World* world = World::New_World(options);

	uint64_t world_id = world->World_ID();
	m_loaded_worlds[world_id] = world;
	m_default_world = world;
}

bool WorldController::World_Exists(uint64_t world_id)
{
	return m_loaded_worlds.contains(world_id);
}

World* WorldController::Get_World(uint64_t world_id)
{
	if (!World_Exists(world_id)) {
		return nullptr;
	}

	return m_loaded_worlds[world_id];
}
