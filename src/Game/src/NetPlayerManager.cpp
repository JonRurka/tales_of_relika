#include "NetPlayerManager.h"

NetPlayerManager* NetPlayerManager::m_instance{nullptr};

void NetPlayerManager::RegisterLocalPlayer(LocalPlayerCharacter* local_player)
{
}

void NetPlayerManager::OnUpdateOrientations(Data data)
{
}

void NetPlayerManager::OnPlayerEvent(Data data)
{
}

void NetPlayerManager::Init()
{
	m_instance = this;

}

void NetPlayerManager::Update(float dt)
{


}

void NetPlayerManager::send_player_location()
{
}

void NetPlayerManager::add_net_commands()
{
}
