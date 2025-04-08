#include "Input.h"

#include "opengl.h"
#include "Graphics.h"

using namespace input;

#define REPEAT_TIMER 0.1f

Input* Input::m_instance{nullptr};

namespace {
	inline std::unordered_map<int, KeyCode> create_key_lookup()
	{
		static const std::unordered_map<int, KeyCode> key_lookup =
		{
			{GLFW_KEY_SPACE, KeyCode::Space},
			{GLFW_KEY_APOSTROPHE, KeyCode::Apostrophe},
			{GLFW_KEY_COMMA, KeyCode::Comma},
			{GLFW_KEY_MINUS, KeyCode::Minus},
			{GLFW_KEY_PERIOD, KeyCode::Period},
			{GLFW_KEY_SLASH, KeyCode::Slash},
			{GLFW_KEY_0, KeyCode::_0},
			{GLFW_KEY_1, KeyCode::_1},
			{GLFW_KEY_2, KeyCode::_2},
			{GLFW_KEY_3, KeyCode::_3},
			{GLFW_KEY_4, KeyCode::_4},
			{GLFW_KEY_5, KeyCode::_5},
			{GLFW_KEY_6, KeyCode::_6},
			{GLFW_KEY_7, KeyCode::_7},
			{GLFW_KEY_8, KeyCode::_8},
			{GLFW_KEY_9, KeyCode::_9},
			{GLFW_KEY_SEMICOLON, KeyCode::Semicolon},
			{GLFW_KEY_EQUAL, KeyCode::Equal},
			{GLFW_KEY_A, KeyCode::A},
			{GLFW_KEY_B, KeyCode::B},
			{GLFW_KEY_C, KeyCode::C},
			{GLFW_KEY_D, KeyCode::D},
			{GLFW_KEY_E, KeyCode::E},
			{GLFW_KEY_F, KeyCode::F},
			{GLFW_KEY_G, KeyCode::G},
			{GLFW_KEY_H, KeyCode::H},
			{GLFW_KEY_I, KeyCode::I},
			{GLFW_KEY_J, KeyCode::J},
			{GLFW_KEY_K, KeyCode::K},
			{GLFW_KEY_L, KeyCode::L},
			{GLFW_KEY_M, KeyCode::M},
			{GLFW_KEY_N, KeyCode::N},
			{GLFW_KEY_O, KeyCode::O},
			{GLFW_KEY_P, KeyCode::P},
			{GLFW_KEY_Q, KeyCode::Q},
			{GLFW_KEY_R, KeyCode::R},
			{GLFW_KEY_S, KeyCode::S},
			{GLFW_KEY_T, KeyCode::T},
			{GLFW_KEY_U, KeyCode::U},
			{GLFW_KEY_V, KeyCode::V},
			{GLFW_KEY_W, KeyCode::W},
			{GLFW_KEY_X, KeyCode::X},
			{GLFW_KEY_Y, KeyCode::Y},
			{GLFW_KEY_Z, KeyCode::Z},
			{GLFW_KEY_LEFT_BRACKET, KeyCode::LeftBracket},
			{GLFW_KEY_BACKSLASH, KeyCode::Backslash},
			{GLFW_KEY_RIGHT_BRACKET, KeyCode::RightBracket},
			{GLFW_KEY_GRAVE_ACCENT, KeyCode::GraveAccent},
			{GLFW_KEY_ESCAPE, KeyCode::Escape},
			{GLFW_KEY_ENTER, KeyCode::Enter},
			{GLFW_KEY_TAB, KeyCode::Tab},
			{GLFW_KEY_BACKSPACE, KeyCode::Backspace},
			{GLFW_KEY_INSERT, KeyCode::Insert},
			{GLFW_KEY_DELETE, KeyCode::DelKey},
			{GLFW_KEY_RIGHT, KeyCode::Right},
			{GLFW_KEY_LEFT, KeyCode::Left},
			{GLFW_KEY_DOWN, KeyCode::Down},
			{GLFW_KEY_UP, KeyCode::Up},
			{GLFW_KEY_PAGE_UP, KeyCode::PageUp},
			{GLFW_KEY_PAGE_DOWN, KeyCode::PageDown},
			{GLFW_KEY_HOME, KeyCode::Home},
			{GLFW_KEY_END, KeyCode::End},
			{GLFW_KEY_CAPS_LOCK, KeyCode::CapsLock},
			{GLFW_KEY_SCROLL_LOCK, KeyCode::ScrollLock},
			{GLFW_KEY_NUM_LOCK, KeyCode::NumLock},
			{GLFW_KEY_PRINT_SCREEN, KeyCode::PrintScreen},
			{GLFW_KEY_PAUSE, KeyCode::Pause},
			{GLFW_KEY_F1, KeyCode::F1},
			{GLFW_KEY_F2, KeyCode::F2},
			{GLFW_KEY_F3, KeyCode::F3},
			{GLFW_KEY_F4, KeyCode::F4},
			{GLFW_KEY_F5, KeyCode::F5},
			{GLFW_KEY_F6, KeyCode::F6},
			{GLFW_KEY_F7, KeyCode::F7},
			{GLFW_KEY_F8, KeyCode::F8},
			{GLFW_KEY_F9, KeyCode::F9},
			{GLFW_KEY_F10, KeyCode::F10},
			{GLFW_KEY_F11, KeyCode::F11},
			{GLFW_KEY_F12, KeyCode::F12},
			{GLFW_KEY_KP_0, KeyCode::KP_0},
			{GLFW_KEY_KP_1, KeyCode::KP_1},
			{GLFW_KEY_KP_2, KeyCode::KP_2},
			{GLFW_KEY_KP_3, KeyCode::KP_3},
			{GLFW_KEY_KP_4, KeyCode::KP_4},
			{GLFW_KEY_KP_5, KeyCode::KP_5},
			{GLFW_KEY_KP_6, KeyCode::KP_6},
			{GLFW_KEY_KP_7, KeyCode::KP_7},
			{GLFW_KEY_KP_8, KeyCode::KP_8},
			{GLFW_KEY_KP_9, KeyCode::KP_9},
			{GLFW_KEY_KP_DECIMAL, KeyCode::KP_Decimal},
			{GLFW_KEY_KP_DIVIDE, KeyCode::KP_Divide},
			{GLFW_KEY_KP_MULTIPLY, KeyCode::KP_Multiply},
			{GLFW_KEY_KP_SUBTRACT, KeyCode::KP_Subtract},
			{GLFW_KEY_KP_ADD, KeyCode::KP_Add},
			{GLFW_KEY_KP_ENTER, KeyCode::KP_Enter},
			{GLFW_KEY_KP_EQUAL, KeyCode::KP_Equal},
			{GLFW_KEY_LEFT_SHIFT, KeyCode::LeftShift},
			{GLFW_KEY_LEFT_CONTROL, KeyCode::LeftControl},
			{GLFW_KEY_LEFT_ALT, KeyCode::LeftAlt},
			{GLFW_KEY_RIGHT_SHIFT, KeyCode::RightShift},
			{GLFW_KEY_RIGHT_CONTROL, KeyCode::RightControl},
			{GLFW_KEY_RIGHT_ALT, KeyCode::RightAlt},
		};

		return key_lookup;
	}

	inline KeyCode translate_key_code(std::unordered_map<int, KeyCode> key_lookup, int key) {
		auto key_it = key_lookup.find(key);

		if (key_it == key_lookup.end())
		{
			return KeyCode::Unknown;
		}

		return key_it->second;
	}

	inline KeyAction translate_key_action(int action)
	{
		if (action == GLFW_PRESS)
		{
			return KeyAction::Down;
		}
		else if (action == GLFW_RELEASE)
		{
			return KeyAction::Up;
		}
		else if (action == GLFW_REPEAT)
		{
			return KeyAction::Repeat;
		}

		return KeyAction::Unknown;
	}

	inline MouseButton translate_mouse_button(int button)
	{
		if (button < GLFW_MOUSE_BUTTON_6)
		{
			return static_cast<MouseButton>(button);
		}

		return MouseButton::Unknown;
	}

	inline MouseAction translate_mouse_action(int action)
	{
		if (action == GLFW_PRESS)
		{
			return MouseAction::Down;
		}
		else if (action == GLFW_RELEASE)
		{
			return MouseAction::Up;
		}

		return MouseAction::Unknown;
	}

}

Input::Input() 
{
	m_instance = this;
	m_key_lookup = create_key_lookup();
}

void Input::update(float dt)
{
	update_keys(dt);
	update_mouse_move(dt);
	update_mouse_press(dt);
}

void Input::update_keys(float dt)
{
	std::vector<input::KeyInputEvent> to_released;
	for (auto& pair : m_input_down_keys) {

		if (pair.second.get_num_frames() > 0) {
			to_released.push_back(pair.second);
		}
		pair.second.tick();
	}

	for (auto& key_input : to_released) {
		m_input_pressed_keys[key_input.get_code()] = key_input;
		key_input.update_action(KeyAction::Repeat);
		key_input.reset();
		m_input_down_keys.erase(key_input.get_code());
	}

	for (auto& pair : m_input_pressed_keys) {
		pair.second.tick();
	}


	std::vector<input::KeyInputEvent> remove_from_up;
	for (auto& pair : m_input_up_keys) {

		if (pair.second.get_num_frames() > 0) {
			remove_from_up.push_back(pair.second);
		}
		pair.second.tick();
	}

	for (auto& key_input : remove_from_up) {
		m_input_up_keys.erase(key_input.get_code());
	}
}

void Input::update_mouse_move(float dt)
{
	if (m_frames_mouse_moved > 2) {
		m_mouse_dx = 0;
		m_mouse_dy = 0;
	}
	m_frames_mouse_moved++;
}

void Input::update_mouse_press(float dt)
{
	std::vector<input::MouseButtonInputEvent> to_released;
	for (auto& pair : m_mouse_input_down_keys) {

		if (pair.second.get_num_frames() > 0) {
			to_released.push_back(pair.second);
		}
		pair.second.tick();
	}

	for (auto& key_input : to_released) {
		m_mouse_input_pressed_keys[key_input.get_button()] = key_input;
		key_input.update_action(MouseAction::Repeat);
		key_input.reset();
		m_mouse_input_down_keys.erase(key_input.get_button());
	}

	for (auto& pair : m_mouse_input_down_keys) {
		pair.second.tick();
	}

	std::vector<input::MouseButtonInputEvent> remove_from_up;
	for (auto& pair : m_mouse_input_up_keys) {

		if (pair.second.get_num_frames() > 0) {
			remove_from_up.push_back(pair.second);
		}
		pair.second.tick();
	}

	for (auto& key_input : remove_from_up) {
		m_mouse_input_up_keys.erase(key_input.get_button());
	}
}

void Input::key_callback(GLFWwindow* window, int key, int, int action, int)
{
	KeyCode   key_code = translate_key_code(m_key_lookup, key);
	KeyAction key_action = translate_key_action(action);
	KeyInputEvent input_event = KeyInputEvent{ key_code, key_action };

	if (key_action == KeyAction::Down) {
		if (!m_input_down_keys.contains(key_code) &&
			!m_input_pressed_keys.contains(key_code) &&
			!m_input_up_keys.contains(key_code))
			m_input_down_keys[key_code] = input_event;
	}
	else if (key_action == KeyAction::Up) {
		if (m_input_down_keys.contains(key_code)) {
			m_input_down_keys.erase(key_code);
		}
		if (m_input_pressed_keys.contains(key_code)) {
			m_input_pressed_keys.erase(key_code);
		}
		m_input_up_keys[key_code] = input_event;
	}

	//Logger::LogDebug(LOG_POS("key_callback"), "Key Action: %s", input_event.to_string().c_str());
	//OnKeyInput_delegate.Call(KeyInputEvent{ key_code, key_action });
}

void Input::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	m_mouse_dx = xpos - m_mouse_xpos;
	m_mouse_dy = ypos - m_mouse_ypos;
	m_mouse_xpos = xpos;
	m_mouse_ypos = ypos;
	m_frames_mouse_moved = 0;

	/*OnMouseMoveInput_delegate.Call(MouseButtonInputEvent{
			MouseButton::Unknown,
			MouseAction::Move,
			static_cast<float>(xpos),
			static_cast<float>(ypos) });*/
}

void Input::mouse_button_callback(GLFWwindow* window, int button, int action, int)
{
	MouseAction mouse_action = translate_mouse_action(action);
	MouseButton mouse_button = translate_mouse_button(button);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	MouseButtonInputEvent input_event = MouseButtonInputEvent{
			mouse_button,
			mouse_action,
			static_cast<float>(xpos),
			static_cast<float>(ypos) };

	if (mouse_action == MouseAction::Down) {
		if (!m_mouse_input_down_keys.contains(mouse_button) &&
			!m_mouse_input_pressed_keys.contains(mouse_button) &&
			!m_mouse_input_up_keys.contains(mouse_button))
			m_mouse_input_down_keys[mouse_button] = input_event;
	}
	else if (mouse_action == MouseAction::Up) {
		if (m_mouse_input_down_keys.contains(mouse_button)) {
			m_mouse_input_down_keys.erase(mouse_button);
		}
		if (m_mouse_input_pressed_keys.contains(mouse_button)) {
			m_mouse_input_pressed_keys.erase(mouse_button);
		}
		m_mouse_input_up_keys[mouse_button] = input_event;
	}

	/*OnMouseButtonInput_delegate.Call();*/
}

bool Input::get_key_down(KeyCode key)
{
	return m_input_down_keys.contains(key);
}

bool Input::get_key(KeyCode key)
{
	if (m_input_pressed_keys.contains(key)) {
		return m_input_pressed_keys[key].get_press_time() > REPEAT_TIMER;
	}
	return false;
}

bool Input::get_key_up(KeyCode key)
{
	return m_input_up_keys.contains(key);
}

bool Input::get_mouse_key_down(input::MouseButton key)
{
	return m_mouse_input_down_keys.contains(key);
}

bool Input::get_mouse_key(input::MouseButton key)
{
	if (m_mouse_input_pressed_keys.contains(key)) {
		return m_mouse_input_pressed_keys[key].get_press_time() > REPEAT_TIMER;
	}
	return false;
}

bool Input::get_mouse_key_up(input::MouseButton key)
{
	return m_mouse_input_up_keys.contains(key);
}

void Input::set_mouse_visibility(bool visible)
{
	Graphics::Instance()->set_mouse_visibility(visible);
}

double Input::get_input_x(std::string device)
{
	return m_mouse_dx * m_mouse_sensitivity;
}

double Input::get_input_y(std::string device)
{
	return m_mouse_dy * m_mouse_sensitivity;
}

glm::vec2 Input::get_mouse_pos()
{
	return glm::vec2(m_mouse_xpos, m_mouse_ypos);
}
