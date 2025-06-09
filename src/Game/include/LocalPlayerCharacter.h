#pragma once

#include "game_engine.h"

#include "Network/OpCodes.h"
#include "Network/Data.h"

#include <vector>

class CapsuleCollider;

class LocalPlayerCharacter : public Component {
public:


	static LocalPlayerCharacter* Get_Instance() {
		return m_instance;
	}

	void SendJumpEvent();

	void SendPlayerEvent(OpCodes::Player_Events event_cmd);

	void SendPlayerEvent(OpCodes::Player_Events event_cmd, std::vector<uint8_t> data);

protected:
	void Init() override;

	void Update(float dt) override;

private:

	static LocalPlayerCharacter* m_instance;

	glm::vec3 m_location;

	CapsuleCollider* m_capsule_collider{ nullptr };

	void jump_control();

	void move_control();

	void look_control();

	void Send(OpCodes::Server cmd, std::vector<uint8_t> data, Protocal type = Protocal_Tcp);


};