#include "LocalPlayerCharacter.h"

#include "GameClient.h"
#include "Network/NetClient.h"
#include "Network/BufferUtils.h"
#include "Physics.h"
#include "CapsuleCollider.h"
#include "CharacterCollider.h"
#include "Standard_Material.h"
#include "Game_Resources.h"
#include "Primitives.h"
#include "Character_HUD.h"

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
#include"BulletCollision/CollisionDispatch/btGhostObject.h"
#include"BulletDynamics/Character/btKinematicCharacterController.h"
#endif

#define MOVE_SEND_TIMEOUT (1. / 20.f)

LocalPlayerCharacter* LocalPlayerCharacter::m_instance{nullptr};

void LocalPlayerCharacter::Init()
{
	m_instance = this;

	m_body_trans = Object().Get_Transform_Ptr();
	//m_body_trans->Position(glm::vec3(100, 50, 100));
	//m_body_trans.lock()->Position(glm::vec3(0, 15, 0));

	m_location = m_body_trans.lock()->Position() + glm::vec3(0, 0.1f, 0);
	m_old_location = m_location.load();

	

	m_capsule_collider = Object().Add_Component<CharacterCollider>();
	m_capsule_collider.lock()->Mass(50.0);
	m_capsule_collider.lock()->Activate();
	m_controller_created = true;
	
	//m_capsule_collider->RigidBody()->setAngularFactor(btVector3(1.0f, 1.0f, 1.0f));

	GameClient::Instance()->Net_Client().AddCommand(OpCodes::Client::Sync_Player_Orientation, OnOrientationSync_cb, this);

	init_geometry();


}

void LocalPlayerCharacter::Update(float dt)
{
	assert(!m_capsule_collider.expired());

	//CharacterCollider& char_col = *m_capsule_collider.lock().get();


	if (Input::GetKeyDown(KeyCode::Escape)) {
		m_mouse_hidden = !m_mouse_hidden;
		Input::Set_Mouse_Visibility(m_mouse_hidden);
	}

	jump_control(dt);
	move_control(dt);
	look_control(dt);

	if (Utilities::Get_Time() - m_debug_time > 1.0f)
	{
		m_debug_time = Utilities::Get_Time();

		//glm::vec3 pos = m_body_trans.lock()->Position();
		//Logger::LogDebug(LOG_POS("Update"), "(%.2lf, %.2lf, %.2lf) == (%.2lf, %.2lf, %.2lf), %.2lf",
		//	m_server_loc.x, m_server_loc.y, m_server_loc.z,
		//	pos.x, pos.y, pos.z, glm::distance(m_server_loc, pos));



		//Logger::LogDebug(LOG_POS("Update"), "Current Pos: (%f, %f, %f), Current Velocity: (%f, %f, %f), Server Pos: (%f, %f, %f)",
		//	m_body_trans->Position().x, m_body_trans->Position().y, m_body_trans->Position().z,
		//	m_velocity.x, m_velocity.y, m_velocity.z,
		//	m_server_loc.x, m_server_loc.y, m_server_loc.z);
	}
	//Graphics::DrawDebugRay(m_server_loc, glm::vec3(0, 3, 0), glm::vec3(1, 1, 0));
}

void LocalPlayerCharacter::FixedUpdate(float dt)
{
	if (!m_controller_created)
		return;

	assert(!m_capsule_collider.expired());
	CharacterCollider& char_col = *m_capsule_collider.lock().get();

	if (!char_col.Character_Inited())
	{
		return;
	}

	if (m_is_locked && m_received_server_pos)
	{
		char_col.Set_Location(m_server_loc);
		return;
	}

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	if (m_do_move) {
		glm::vec3 tr_move_vec = glm::vec3(move_vec.x, 0, move_vec.z);

		if (char_col.Get_Controller().onGround())
			char_col.Get_Controller().setWalkDirection(btVector3(tr_move_vec.x, tr_move_vec.y, tr_move_vec.z).normalized() / 10);
		else
			char_col.Get_Controller().setWalkDirection(btVector3(tr_move_vec.x, tr_move_vec.y, tr_move_vec.z).normalized() / 10);

	}
	else {
		char_col.Get_Controller().setWalkDirection(btVector3(0, 0, 0));
	}

	btVector3 pos = char_col.Get_Controller().getGhostObject()->getWorldTransform().getOrigin();
	m_location = glm::fvec3(pos.x(), pos.y(), pos.z());

	btVector3 vel = char_col.Get_Controller().getLinearVelocity();
	m_velocity = glm::fvec3(vel.x(), vel.y(), vel.z());// (m_location - m_old_location) / dt;
#else
	if (true)
	{
		//if (m_do_move)
			//Logger::LogDebug(LOG_POS("move_control"), "Move: (%f, %f)",
			//	move_vec.x, move_vec.z);
		glm::vec3 m_vec = m_move_vec.load();
		Vec3 tr_move_vec = Vec3(m_vec.x, 0, m_vec.z);
		if (tr_move_vec != Vec3::sZero()) {
			tr_move_vec = tr_move_vec.Normalized();
		}

		char_col.HandleMovement(tr_move_vec, dt);
	}
	else {
		//char_col.HandleMovement(Vec3(0, 0, 0), dt);
	}

	Vec3 pos = char_col.Get_Controller().GetPosition();
	Vec3 c_up = char_col.Get_Controller().GetUp();
	m_old_location = m_location.load();
	m_location = glm::fvec3(pos.GetX(), pos.GetY(), pos.GetZ());
	glm::vec3 up = glm::vec3(c_up.GetX(), c_up.GetY(), c_up.GetZ());

	//Logger::LogDebug(LOG_POS("move_control"), "Position(%i): (%f, %f, %f)",
	//	(int)char_col.Get_Controller().IsSupported(), up.x, up.y, up.z);

	if (m_should_jump.load()) {
		char_col.Jump(5.0);
		m_should_jump = false;
	}


	Vec3 vel = char_col.Get_Controller().GetLinearVelocity();
	m_velocity = glm::fvec3(vel.GetX(), vel.GetY(), vel.GetZ());


	glm::vec3 pred_server_pos = (m_server_loc + glm::vec3(0, 0.0f, 0)) + (m_velocity.load() * (float)m_move_trip_time);
	glm::vec3 new_pos = glm::mix(m_location.load(), pred_server_pos, dt * 2);
	char_col.Set_Location(new_pos);

#endif

}

void LocalPlayerCharacter::OnDestroy()
{
}

void LocalPlayerCharacter::OnEnabled()
{

}

void LocalPlayerCharacter::OnDisabled()
{

}

void LocalPlayerCharacter::jump_control(float dt)
{
	assert(!m_capsule_collider.expired());

	if (!m_capsule_collider.lock()->Character_Inited())
	{
		return;
	}

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	if (Input::GetKeyDown(KeyCode::Space) && 
		m_capsule_collider.lock()->Get_Controller().onGround())
	{
		m_capsule_collider.lock()->Get_Controller().jump(btVector3(0, m_jump_power, 0));
		SendPlayerEvent(OpCodes::Player_Events::Jump, Protocal_Tcp);
		//m_capsule_collider->RigidBody()->applyCentralImpulse(btVector3(0, m_jump_force, 0));
	}
#else
	if (Input::GetKeyDown(input::KeyCode::Space))
	{
		SendJumpEvent();
		m_should_jump = true;
	}


#endif
}

void LocalPlayerCharacter::move_control(float dt)
{
	assert(!m_cam_trans.expired());
	assert(!m_capsule_collider.expired());

	if (!m_capsule_collider.lock()->Character_Inited())
	{
		return;
	}

	if (m_is_locked && m_received_server_pos)
	{
		m_body_trans.lock()->Position(m_server_loc + glm::vec3(0, 0.1f, 0));
		return;
	}

	//CharacterCollider& char_col = *m_capsule_collider.lock().get();

	glm::vec3 dir_forward = m_cam_trans.lock()->Forward();
	glm::vec3 dir_right = m_cam_trans.lock()->Right();

	glm::vec3 forward = glm::normalize(glm::vec3(dir_forward.x, 0, dir_forward.z));
	glm::vec3 right = -glm::normalize(glm::vec3(dir_right.x, 0, dir_right.z));

	m_do_move = false;
	glm::vec3 m_vec = glm::vec3(0, 0, 0);
	//m_move_vec = glm::vec3(0, 0, 0);
	if (Input::GetKey(input::KeyCode::W)) {
		//Logger::LogDebug(LOG_POS("move_control"), "move forward");
		m_do_move = true;
		m_vec += forward;
		//Logger::LogDebug(LOG_POS("move_control"), "W");
	}
	if (Input::GetKey(input::KeyCode::S)) {
		m_do_move = true;
		m_vec -= forward;
		//Logger::LogDebug(LOG_POS("move_control"), "S");
	}
	if (Input::GetKey(input::KeyCode::A)) {
		m_do_move = true;
		m_vec += right;
		//Logger::LogDebug(LOG_POS("move_control"), "A");
	}
	if (Input::GetKey(input::KeyCode::D)) {
		m_do_move = true;
		m_vec -= right;
		//Logger::LogDebug(LOG_POS("move_control"), "D");
	}
	m_move_vec = m_vec;

	double curr_time = Utilities::Get_Time();
	if (curr_time - m_last_send_move >= MOVE_SEND_TIMEOUT)
	{
		m_move_send_id++;

		m_last_send_move = Utilities::Get_Time();
		std::vector<uint8_t> send_data;
		send_data.push_back((m_do_move ? 0x01 : 0x00));
		send_data = BufferUtils::AppendFloat(send_data, m_vec.x);
		send_data = BufferUtils::AppendFloat(send_data, m_vec.z);
		// TODO: heading
		send_data = BufferUtils::Append_UInt64(send_data, m_move_send_id);
		
		m_net_trip_times[m_move_send_id] = Utilities::Get_Time();

		SendPlayerEvent(OpCodes::Player_Events::Process_Move, send_data, Protocal_Udp);
		//Logger::LogDebug(LOG_POS("move_control"), "Send Move state");
	}

	

	return;
	/*if (m_received_server_pos && m_do_move)
	{
		glm::vec3 pred_server_pos = (m_server_loc + glm::vec3(0, 0.0f, 0)) + (m_velocity.load() * (float)m_move_trip_time);

		if (m_do_move)
		{
			glm::vec3 new_pos = glm::mix(m_location.load(), pred_server_pos, dt * 2);
			//char_col.Set_Location(new_pos);
			m_body_trans.lock()->Position(new_pos);
			m_cam_trans.lock()->Position(new_pos + cam_offset);
			m_location = new_pos;
			return;
		}


		// Start the process of moving back they player if they desync.
		if (!m_moving_player_back)
		{
			if (glm::distance(m_location.load(), pred_server_pos) > 0.5f)
			{
				m_moving_player_back = true;
				move_dt = 1.0;
				Logger::LogDebug(LOG_POS("move_control"), "Player location desynced with server... syncing postion.");
				return;
			}
		}
		else
		{
			move_dt += dt;
			glm::vec3 new_pos = glm::mix(m_location.load(), pred_server_pos, move_dt);
			//char_col.Set_Location(new_pos);
			m_body_trans.lock()->Position(new_pos);
			m_cam_trans.lock()->Position(new_pos + cam_offset);
			m_location = new_pos;

			if (glm::distance(m_location.load(), pred_server_pos) < 0.1f) {
				m_moving_player_back = false;
				Logger::LogDebug(LOG_POS("move_control"), "Stop syncing position now.");
			}
		}
	}*/

	//Graphics::DrawDebugRay(m_body_trans->Position(), forward * 5.0f, glm::vec3(1, 0, 0));

}

void LocalPlayerCharacter::OnOrientationSync(Data data)
{
	//return;

	auto data_buf = data.Buffer;
	//float* orientation_buff = (float*)(data.Buffer.data());
	//glm::vec3 player_loc = glm::vec3(orientation_buff[0], orientation_buff[1], orientation_buff[2]);
	//glm::quat player_rot = glm::quat(orientation_buff[3], orientation_buff[4], orientation_buff[5], orientation_buff[6]);

	float loc_x = *((float*)data_buf.data());
	data_buf = BufferUtils::RemoveFront(Remove_Float, data_buf);

	float loc_y = *((float*)data_buf.data());
	data_buf = BufferUtils::RemoveFront(Remove_Float, data_buf);

	float loc_z = *((float*)data_buf.data());
	data_buf = BufferUtils::RemoveFront(Remove_Float, data_buf);

	uint64_t move_id = *((uint64_t*)data_buf.data());
	data_buf = BufferUtils::RemoveFront(Remove_UInt64, data_buf);

	glm::vec3 player_loc = glm::vec3(loc_x, loc_y, loc_z);

	//Logger::LogDebug(LOG_POS("OnOrientationSync"), "(%f, %f, %f). recv queue size: %i",
	//	player_loc.x, player_loc.y, player_loc.z, GameClient::Instance()->Net_Client()->UDP_Recv_Q_Size());
	m_server_loc = player_loc;

	if (m_net_trip_times.contains(move_id))
	{
		double m_sent_time = m_net_trip_times[move_id];
		m_move_trip_time = Utilities::Get_Time() - m_sent_time;
		m_net_trip_times.erase(move_id);
		m_received_server_pos = true;
		//Logger::LogDebug(LOG_POS("OnOrientationSync"), "Approx Trip Time: %f", m_move_trip_time);
	}


}

void LocalPlayerCharacter::look_control(float dt)
{
	assert(!m_cam_trans.expired());

	if (m_mouse_hidden)
	{
		float mouse_x = Input::Get_Input_X();
		float mouse_y = Input::Get_Input_Y();
		update_rotation(dt, mouse_x, mouse_y);
	}
	m_cam_trans.lock()->Position(m_location.load() + cam_offset);
}

void LocalPlayerCharacter::init_geometry()
{
	m_character_material = std::make_shared<Standard_Material>();
	m_character_material->Set_Shader(Shader::Get_Shader("standard"));
	m_character_material->SetVec3("material_ambientColor", glm::vec3(1.0f, 0.5f, 0.31f));
	m_character_material->SetVec3("material_diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_character_material->SetVec2("material_scale", glm::vec2(32.0f, 32.0f));
	m_character_material->setFloat("material_shininess", 32.0f);
	m_character_material->setFloat("material_specular_intensity", 1.0f);
	m_character_material->SetVec3("globalAmbientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_character_material->setFloat("globalAmbientIntensity", 0.1f);
	m_character_material->setTexture("material_diffuse", Game_Resources::Textures::CONTAINER_DIFFUSE);
	m_character_material->setTexture("material_specular", Game_Resources::Textures::CONTAINER_SPECULAR);

	glm::vec4 cube_color(1.0f, 1.0f, 1.0f, 1.0f);
	std::vector<glm::vec4> floor_cube_colors;
	floor_cube_colors.assign(Primitives::Capsule_Vertices.size(), cube_color);

	Mesh::Shared cube_mesh = Mesh::Create();
	cube_mesh->Vertices(Primitives::Capsule_Vertices);
	cube_mesh->Normals(Primitives::Capsule_Normals);
	cube_mesh->Colors(floor_cube_colors);
	cube_mesh->Indices(Primitives::Capsule_Triangles);
	//cube_mesh->TexCoords(floor_tex_coords);
	//cube_mesh->Activate();

	Object().Get_MeshRenderer().Set_Mesh(cube_mesh);
	//Object()->Get_MeshRenderer()->Set_Material(m_character_material);
	//Object()->Get_Transform()->Scale(1, 1.25, 1);
}


void LocalPlayerCharacter::Set_Camera_Object(WorldObject::Weak cam_object)
{
	assert(!m_body_trans.expired());
	assert(!cam_object.expired());

	Logger::LogDebug(LOG_POS("Set_Camera_Object"), "Set camera");
	Input::Set_Mouse_Visibility(false);
	m_mouse_hidden = true;

	//cam_object->Parent(Object());
	cam_object.lock()->Get_Transform().Position(m_body_trans.lock()->Position() + cam_offset);

	m_cam_trans = cam_object.lock()->Get_Transform_Ptr();
	Input::Mouse_Sensitivity(50);

	m_cam_euler = m_cam_trans.lock()->EulerAngles();
	update_rotation(0, 0, 0);

	m_hud = cam_object.lock()->Add_Component<Character_HUD>();
	m_hud.lock()->Init(Camera::Get_Active_Ptr());
}

void LocalPlayerCharacter::GameStarted()
{
	LockOrientation(false);
	m_hud.lock()->HotBar_Visible(true);
}

void LocalPlayerCharacter::LockOrientation(bool locked)
{
	assert(!m_capsule_collider.expired());
	m_is_locked = locked;
	m_capsule_collider.lock()->Enabled(!locked);
}

void LocalPlayerCharacter::SendJumpEvent()
{
	SendPlayerEvent(OpCodes::Player_Events::Jump);
}

void LocalPlayerCharacter::SendPlayerEvent(OpCodes::Player_Events event_cmd, Protocal protocal)
{
	SendPlayerEvent(event_cmd, std::vector<uint8_t>(), protocal);
}

void LocalPlayerCharacter::SendPlayerEvent(OpCodes::Player_Events event_cmd, std::vector<uint8_t> data, Protocal protocal)
{
	std::vector<uint8_t> send_data;
	send_data.push_back((uint8_t)OpCodes::Server_World::Player_Event);
	send_data.push_back((uint8_t)event_cmd);
	send_data = BufferUtils::Add(send_data, data);
	Send(OpCodes::Server::World_Command, send_data, protocal);
}

void LocalPlayerCharacter::Send(OpCodes::Server cmd, std::vector<uint8_t> data, Protocal type)
{
	GameClient::Instance()->Net_Client().Send(cmd, data, type);
}

LocalPlayerCharacter::Weak LocalPlayerCharacter::get_instance()
{
	return std::dynamic_pointer_cast<LocalPlayerCharacter>(shared_from_this());
}

void LocalPlayerCharacter::update_rotation(float dt, float mouse_x, float mouse_y)
{
	// https://community.khronos.org/t/preventing-camera-from-being-upside-down/72838/3

	assert(!m_cam_trans.expired());

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

	//Logger::LogDebug(LOG_POS("update_rotation"), "(%f, %f)", 
	//	horiz_rad, vert_rad);

	glm::vec3 currentViewingDirection = glm::vec3(
		cos(vert_rad) * sin(horiz_rad),
		sin(vert_rad),
		cos(vert_rad) * cos(horiz_rad)
	);

	glm::quat new_rot = glm::quatLookAt(currentViewingDirection, glm::vec3(0.0f, 1.0f, 0.0f));

	m_cam_trans.lock()->Rotation(new_rot);

}