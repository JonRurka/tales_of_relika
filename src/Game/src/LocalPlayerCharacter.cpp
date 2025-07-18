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

#include"BulletCollision/CollisionDispatch/btGhostObject.h"
#include"BulletDynamics/Character/btKinematicCharacterController.h"

LocalPlayerCharacter* LocalPlayerCharacter::m_instance{nullptr};

void LocalPlayerCharacter::Init()
{
	m_instance = this;

	m_body_trans = Object()->Get_Transform();
	m_body_trans->Position(glm::vec3(0, 50, 0));

	m_capsule_collider = Object()->Add_Component<CharacterCollider>();
	m_capsule_collider->Mass(50.0);
	m_capsule_collider->Activate();
	
	//m_capsule_collider->RigidBody()->setAngularFactor(btVector3(1.0f, 1.0f, 1.0f));

	init_geometry();


}

void LocalPlayerCharacter::Update(float dt)
{
	if (Input::GetKeyDown(KeyCode::Escape)) {
		m_mouse_hidden = !m_mouse_hidden;
		Input::Set_Mouse_Visibility(m_mouse_hidden);
	}

	jump_control(dt);
	move_control(dt);
	look_control(dt);
}

void LocalPlayerCharacter::OnDestroy()
{
}

void LocalPlayerCharacter::jump_control(float dt)
{
	if (Input::GetKeyDown(KeyCode::Space) && 
		m_capsule_collider->Get_Controller()->onGround()) 
	{
		m_capsule_collider->Get_Controller()->jump(btVector3(0, m_jump_power, 0));
		//m_capsule_collider->RigidBody()->applyCentralImpulse(btVector3(0, m_jump_force, 0));
	}
}

void LocalPlayerCharacter::move_control(float dt)
{

	glm::vec3 dir_forward = m_cam_trans->Forward();
	glm::vec3 dir_right = m_cam_trans->Right();

	glm::vec3 forward = glm::normalize(glm::vec3(dir_forward.x, 0, dir_forward.z));
	glm::vec3 right = -glm::normalize(glm::vec3(dir_right.x, 0, dir_right.z));

	bool do_move = false;
	glm::vec3 move_vec = glm::vec3(0, 0, 0);
	if (Input::GetKey(input::KeyCode::W)) {
		//Logger::LogDebug(LOG_POS("move_control"), "move forward");
		do_move = true;
		move_vec += forward;
	}
	if (Input::GetKey(input::KeyCode::S)) {
		do_move = true;
		move_vec -= forward;
	}
	if (Input::GetKey(input::KeyCode::A)) {
		do_move = true;
		move_vec += right;
	}
	if (Input::GetKey(input::KeyCode::D)) {
		do_move = true;
		move_vec -= right;
	}

	
	if (do_move) {
		glm::vec3 tr_move_vec = glm::vec3(move_vec.x, 0, move_vec.z);

		if (m_capsule_collider->Get_Controller()->onGround())
			m_capsule_collider->Get_Controller()->setWalkDirection(btVector3(tr_move_vec.x, tr_move_vec.y, tr_move_vec.z).normalized() / 10);
		else
			m_capsule_collider->Get_Controller()->setWalkDirection(btVector3(tr_move_vec.x, tr_move_vec.y, tr_move_vec.z).normalized() / 10);

	}
	else {
		m_capsule_collider->Get_Controller()->setWalkDirection(btVector3(0, 0, 0));
	}

	//Graphics::DrawDebugRay(m_body_trans->Position(), forward * 5.0f, glm::vec3(1, 0, 0));

}

void LocalPlayerCharacter::look_control(float dt)
{
	if (m_mouse_hidden)
	{
		float mouse_x = Input::Get_Input_X();
		float mouse_y = Input::Get_Input_Y();
		update_rotation(dt, mouse_x, mouse_y);
	}
	m_cam_trans->Position(m_body_trans->Position() + cam_offset);
}

void LocalPlayerCharacter::init_geometry()
{
	m_character_material = new Standard_Material();
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

	Mesh* cube_mesh = new Mesh();
	cube_mesh->Vertices(Primitives::Capsule_Vertices);
	cube_mesh->Normals(Primitives::Capsule_Normals);
	cube_mesh->Colors(floor_cube_colors);
	cube_mesh->Indices(Primitives::Capsule_Triangles);
	//cube_mesh->TexCoords(floor_tex_coords);
	//cube_mesh->Activate();

	//Object()->Get_MeshRenderer()->Set_Mesh(cube_mesh);
	//Object()->Get_MeshRenderer()->Set_Material(m_character_material);
	//Object()->Get_Transform()->Scale(1, 1.25, 1);
}


void LocalPlayerCharacter::Set_Camera_Object(WorldObject* cam_object)
{
	Logger::LogDebug(LOG_POS("Set_Camera_Object"), "Set camera");
	Input::Set_Mouse_Visibility(false);
	m_mouse_hidden = true;

	//cam_object->Parent(Object());
	cam_object->Get_Transform()->Position(m_body_trans->Position() + cam_offset);

	m_cam_trans = cam_object->Get_Transform();
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

	//Logger::LogDebug(LOG_POS("update_rotation"), "(%f, %f)", 
	//	horiz_rad, vert_rad);

	glm::vec3 currentViewingDirection = glm::vec3(
		cos(vert_rad) * sin(horiz_rad),
		sin(vert_rad),
		cos(vert_rad) * cos(horiz_rad)
	);

	glm::quat new_rot = glm::quatLookAt(currentViewingDirection, glm::vec3(0.0f, 1.0f, 0.0f));

	m_cam_trans->Rotation(new_rot);

}