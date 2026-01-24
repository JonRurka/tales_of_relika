#pragma once

#include "game_engine.h"

#include "Network/OpCodes.h"
#include "Network/Data.h"

#include "Standard_Material.h"
#include "Character_HUD.h"

#include <vector>
#include <atomic>

#define CAM_OFFSET_DEFAULT (glm::vec3(0, 1.2, 0))
#define JUMP_POWER_DEFAULT (6.0)

class CharacterCollider;
class Standard_Material;
class Character_HUD;

class LocalPlayerCharacter : public Component {
public:
	typedef std::shared_ptr<LocalPlayerCharacter> Shared;
	typedef std::weak_ptr<LocalPlayerCharacter> Weak;
	//struct

	static LocalPlayerCharacter::Weak Get_Instance() {
		assert(m_instance != nullptr);
		return m_instance->get_instance();
	}

	void Set_Camera_Object(WorldObject::Weak cam_object);

	void LockOrientation(bool locked);

	void SendJumpEvent();

	void SendPlayerEvent(OpCodes::Player_Events event_cmd, Protocal protocal = Protocal_Tcp);

	void SendPlayerEvent(OpCodes::Player_Events event_cmd, std::vector<uint8_t> data, Protocal protocal = Protocal_Tcp);

	static void OnOrientationSync_cb(void* obj, Data data) {
		m_instance->OnOrientationSync(data);
	}
	void OnOrientationSync(Data data);

protected:
	void Init() override;

	void Update(float dt) override;

	void FixedUpdate(float dt) override;

	void OnDestroy() override;

private:

	static LocalPlayerCharacter* m_instance;

	std::atomic<glm::vec3> m_old_location{ glm::vec3()};
	std::atomic<glm::vec3> m_location{ glm::vec3() };
	std::atomic<glm::vec3> m_velocity{ glm::vec3() };
	glm::vec3 m_server_loc{ glm::vec3() };
	bool m_do_move{ false };
	uint64_t m_move_send_id{ 0 };

	std::unordered_map<uint64_t, double> m_net_trip_times;
	std::atomic<double> m_move_trip_time{ 0 };
	bool m_received_server_pos{ false };
	bool m_moving_player_back{ false };
	float move_dt{ 0 };
	std::atomic<glm::vec3> m_move_vec;

	volatile bool m_controller_created{ false };

	Transform::Weak m_body_trans;

	CharacterCollider::Weak m_capsule_collider;

	float m_moveSpeed = 10.0f; // movement speed of the character
	float m_turnSpeed = 180.0f; // turn speed of the character
	float m_jump_force = 300.0f;
	Transform::Weak m_cameraTransform; // reference to the camera transform

	Transform::Weak m_cam_trans;
	float m_cam_horizontalAngle{ 0 };
	float m_cam_verticalAngle{ 0 };
	glm::vec3 m_cam_euler{ glm::vec3() };

	glm::vec3 cam_offset{ CAM_OFFSET_DEFAULT };
	float m_jump_power{ JUMP_POWER_DEFAULT };

	//btQuaternion m_body_quat;
	glm::quat m_body_quat;
	glm::vec3 m_look_dir_forward;
	glm::vec3 m_look_dir_right;
	Standard_Material::Shared m_character_material;

	Character_HUD::Weak m_hud;

	double m_last_send_move{ 0 };

	double m_debug_time{ 0 };

	bool m_mouse_hidden{ false };

	Weak get_instance();

	void update_rotation(float dt, float mouse_x, float mouse_y);

	void jump_control(float dt);

	void move_control(float dt);

	void look_control(float dt);

	void init_geometry();

	void Send(OpCodes::Server cmd, std::vector<uint8_t> data, Protocal type = Protocal_Tcp);

	inline static const std::string LOG_LOC{ "LOCAL_PLAYER_CHARACTER" };
};