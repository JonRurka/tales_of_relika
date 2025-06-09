#include "LocalPlayerCharacter.h"

#include "GameClient.h"
#include "Network/NetClient.h"
#include "Network/BufferUtils.h"
#include "Physics.h"
#include "CapsuleCollider.h"


LocalPlayerCharacter* LocalPlayerCharacter::m_instance{nullptr};

void LocalPlayerCharacter::Init()
{
	m_instance = this;

	m_capsule_collider = Object()->Add_Component<CapsuleCollider>();
	
}

void LocalPlayerCharacter::Update(float dt)
{
	jump_control();
	move_control();
	look_control();
}

void LocalPlayerCharacter::jump_control()
{
	
}

void LocalPlayerCharacter::move_control()
{

	glm::vec2 move_vec = glm::vec2(0.0);
	if (Input::GetKeyDown(input::KeyCode::W)) {
		move_vec.y += 1;
	}
	if (Input::GetKeyDown(input::KeyCode::S)) {
		move_vec.y -= 1;
	}
	if (Input::GetKeyDown(input::KeyCode::A)) {
		move_vec.x += 1;
	}
	if (Input::GetKeyDown(input::KeyCode::D)) {
		move_vec.x -= 1;
	}


	m_capsule_collider->RigidBody()->applyCentralImpulse(btVector3(move_vec.x, 0, move_vec.y));

}

void LocalPlayerCharacter::look_control()
{



}


void LocalPlayerCharacter::SendJumpEvent()
{
	SendPlayerEvent(OpCodes::Player_Events::Jump);
}

void LocalPlayerCharacter::SendPlayerEvent(OpCodes::Player_Events event_cmd)
{
	SendPlayerEvent(OpCodes::Player_Events::Jump, std::vector<uint8_t>());
}

void LocalPlayerCharacter::SendPlayerEvent(OpCodes::Player_Events event_cmd, std::vector<uint8_t> data)
{
	std::vector<uint8_t> send_data;
	send_data.push_back((uint8_t)OpCodes::Server_World::Player_Event);
	send_data.push_back((uint8_t)event_cmd);
	send_data = BufferUtils::Add(send_data, data);
	Send(OpCodes::Server::World_Command, send_data);
}

void LocalPlayerCharacter::Send(OpCodes::Server cmd, std::vector<uint8_t> data, Protocal type)
{
	GameClient::Instance()->Net_Client()->Send(cmd, data, type);
}
