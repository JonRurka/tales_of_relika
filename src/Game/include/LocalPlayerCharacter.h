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

	void Set_Camera_Object(WorldObject* cam_object);

	void SendJumpEvent();

	void SendPlayerEvent(OpCodes::Player_Events event_cmd);

	void SendPlayerEvent(OpCodes::Player_Events event_cmd, std::vector<uint8_t> data);

protected:
	void Init() override;

	void Update(float dt) override;

	void OnDestroy() override;

private:

	static LocalPlayerCharacter* m_instance;

	glm::vec3 m_location;

	Transform* m_body_trans{ nullptr };

	CapsuleCollider* m_capsule_collider{ nullptr };

	float m_moveSpeed = 10.0f; // movement speed of the character
	float m_turnSpeed = 180.0f; // turn speed of the character
	Transform* m_cameraTransform; // reference to the camera transform

	Transform* m_cam_trans{ nullptr };
	float m_cam_horizontalAngle{ 0 };
	float m_cam_verticalAngle{ 0 };
	glm::vec3 m_cam_euler;

	void update_rotation(float dt, float mouse_x, float mouse_y);

	void jump_control(float dt);

	void move_control(float dt);

	void look_control(float dt);

	void Send(OpCodes::Server cmd, std::vector<uint8_t> data, Protocal type = Protocal_Tcp);

	inline static const std::string LOG_LOC{ "LOCAL_PLAYER_CHARACTER" };
};