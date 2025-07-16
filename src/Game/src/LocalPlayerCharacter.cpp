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
	
	m_body_trans = Object()->Get_Transform();

	m_body_trans->Position(glm::vec3(0, 20, 0));
}

void LocalPlayerCharacter::Update(float dt)
{
	jump_control(dt);
	move_control(dt);
	look_control(dt);
}

void LocalPlayerCharacter::OnDestroy()
{
}

void LocalPlayerCharacter::jump_control(float dt)
{
	
}

void LocalPlayerCharacter::move_control(float dt)
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

	m_capsule_collider->RigidBody()->translate(btVector3(
		move_vec.x, 0, move_vec.y)
	);

	//m_capsule_collider->RigidBody()->applyCentralImpulse(btVector3(move_vec.x, 0, move_vec.y));

}

void LocalPlayerCharacter::look_control(float dt)
{
	float mouse_x = Input::Get_Input_X();
	float mouse_y = Input::Get_Input_Y();
	update_rotation(dt, mouse_x, mouse_y);
}


void LocalPlayerCharacter::Set_Camera_Object(WorldObject* cam_object)
{
	//cam_object->Parent(Object());
	cam_object->Get_Transform()->Position(glm::vec3(0, 0, 0));

	m_cam_trans = Object()->Get_Transform();
	Input::Mouse_Sensitivity(50);

	m_cam_euler = m_cam_trans->EulerAngles();

	update_rotation(0, 0, 0);
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

void LocalPlayerCharacter::update_rotation(float dt, float mouse_x, float mouse_y)
{
	// https://community.khronos.org/t/preventing-camera-from-being-upside-down/72838/3

	m_cam_euler.y += -mouse_x * dt; // horizontal
	m_cam_euler.x += -mouse_y * dt; // vertical

	if (m_cam_euler.x > 80.0f) {
		m_cam_euler.x = 80.0f;
	}
	else if (m_cam_euler.x < -80.0f) {
		m_cam_euler.x = -80.0f;
	}

	float horiz_rad = glm::radians(m_cam_euler.y);
	float vert_rad = glm::radians(m_cam_euler.x);


	glm::vec3 currentViewingDirection = glm::vec3(
		cos(vert_rad) * sin(horiz_rad),
		sin(vert_rad),
		cos(vert_rad) * cos(horiz_rad)
	);

	glm::quat new_rot = glm::quatLookAt(currentViewingDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	m_cam_trans->Rotation(new_rot);

	//Logger::LogDebug(LOG_POS("Init"), "New Rotation: ( %f, %f)", m_horizontalAngle, m_verticalAngle);
	glm::vec3 dir = m_cam_trans->Forward();
	glm::vec3 euler = m_cam_trans->EulerAngles();

	glm::vec3 body_euler = m_body_trans->EulerAngles();
	m_body_trans->Rotation(body_euler.x, euler.y, body_euler.z);

	m_cam_trans->Position(m_body_trans->Position() + glm::vec3(0, 0.45, 0));
}