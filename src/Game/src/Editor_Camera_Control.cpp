#include "Editor_Camera_Control.h"

#include "Logger.h"

void Editor_Camera_Control::Init()
{
	trans = Object()->Get_Transform();
	Input::Mouse_Sensitivity(50);

	m_euler = trans->EulerAngles();
	//m_horizontalAngle = euler.y;
	//m_verticalAngle = euler.x;
	//Logger::LogDebug(LOG_POS("Init"), "Initial Rotation: ( %f, %f, %f)", euler.x, );
	//glm::vec3 dir = trans->Forward();
	//Logger::LogDebug(LOG_POS("Init"), "Initial Forward: (%f, %f, %f), Euler: (%f, %f, %f)", dir.x, dir.y, dir.z, m_euler.x, m_euler.y, m_euler.z);
	update_rotation(0, 0, 0);
}

void Editor_Camera_Control::Update(float dt)
{
	bool is_holding_left_mouse = false;
	if (Input::GetMouseKeyDown(MouseButton::Right)) {
		Input::Set_Mouse_Visibility(false);
	}
	if (Input::GetMouseKey(MouseButton::Right)) {
		float mouse_x = Input::Get_Input_X();
		float mouse_y = Input::Get_Input_Y();
		update_rotation(dt, mouse_x, mouse_y);
		is_holding_left_mouse = true;
	}
	if (Input::GetMouseKeyUp(MouseButton::Right)) {
		Input::Set_Mouse_Visibility(true);
	}

	if (is_holding_left_mouse) {
		float speed = m_move_speed;

		if (Input::GetKey(KeyCode::LeftShift) || Input::GetKey(KeyCode::RightShift)) {
			speed *= 3;
		}
		if (Input::GetKey(KeyCode::LeftControl) || Input::GetKey(KeyCode::RightControl)) {
			speed *= 1/3.0f;
		}

		if (Input::GetKey(KeyCode::W)) {
			trans->Translate(trans->Forward() * speed * dt);
		}
		if (Input::GetKey(KeyCode::S)) {
			trans->Translate(-trans->Forward() * speed * dt);
		}
		if (Input::GetKey(KeyCode::A)) {
			trans->Translate(-trans->Right() * speed * dt);
		}
		if (Input::GetKey(KeyCode::D)) {
			trans->Translate(trans->Right() * speed * dt);
		}
	}

	
}

void Editor_Camera_Control::update_rotation(float dt, float mouse_x, float mouse_y)
{
	// https://community.khronos.org/t/preventing-camera-from-being-upside-down/72838/3

	m_euler.y += -mouse_x * dt; // horizontal
	m_euler.x += -mouse_y * dt; // vertical

	if (m_euler.x > 80.0f) {
		m_euler.x = 80.0f;
	}
	else if (m_euler.x < -80.0f) {
		m_euler.x = -80.0f;
	}

	float horiz_rad = glm::radians(m_euler.y);
	float vert_rad = glm::radians(m_euler.x);


	glm::vec3 currentViewingDirection = glm::vec3(
		cos(vert_rad) * sin(horiz_rad),
		sin(vert_rad),
		cos(vert_rad) * cos(horiz_rad)
	);

	glm::quat new_rot = glm::quatLookAt(currentViewingDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	trans->Rotation(new_rot);

	//Logger::LogDebug(LOG_POS("Init"), "New Rotation: ( %f, %f)", m_horizontalAngle, m_verticalAngle);
	glm::vec3 dir = trans->Forward();
	glm::vec3 euler = trans->EulerAngles();
	//Logger::LogDebug(LOG_POS("Update"), "New Forward: (%f, %f, %f), Euler: (%f, %f, %f)", dir.x, dir.y, dir.z, euler.x, euler.y, euler.z);
}
