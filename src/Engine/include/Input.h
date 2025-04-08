#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include "input_events.h"
#include "Utilities.h"

class Engine;
class Graphics;
struct GLFWwindow;

class Input {
	friend class Engine;
	friend class Graphics;
public:
	static Input* Instance() { return m_instance; }

	static bool GetKeyDown(input::KeyCode key) {
		return m_instance->get_key_down(key);

	}
	static bool GetKey(input::KeyCode key) {
		return m_instance->get_key(key);
	}
	static bool GetKeyUp(input::KeyCode key) {
		return m_instance->get_key_up(key);
	}
	static bool GetMouseKeyDown(input::MouseButton key) {
		return m_instance->get_mouse_key_down(key);

	}
	static bool GetMouseKey(input::MouseButton key) {
		return m_instance->get_mouse_key(key);
	}
	static bool GetMouseKeyUp(input::MouseButton key) {
		return m_instance->get_mouse_key_up(key);
	}
	static void Set_Mouse_Visibility(bool visible) { m_instance->set_mouse_visibility(visible); }
	static double Get_Input_X(std::string device = "") { return m_instance->get_input_x(device); }
	static double Get_Input_Y(std::string device = "") { return m_instance->get_input_y(device); }



	static glm::vec2 Get_Mouse_Position() { return m_instance->get_mouse_pos(); }

	static void Mouse_Sensitivity(double value) { m_instance->m_mouse_sensitivity = value; }


private:
	Input();

	static Input* m_instance;

	std::unordered_map<int, input::KeyCode> m_key_lookup;

	std::unordered_map<input::MouseButton, input::MouseButtonInputEvent> m_mouse_input_down_keys;
	std::unordered_map<input::MouseButton, input::MouseButtonInputEvent> m_mouse_input_pressed_keys;
	std::unordered_map<input::MouseButton, input::MouseButtonInputEvent> m_mouse_input_up_keys;

	std::unordered_map<input::KeyCode, input::KeyInputEvent> m_input_down_keys;
	std::unordered_map<input::KeyCode, input::KeyInputEvent> m_input_pressed_keys;
	std::unordered_map<input::KeyCode, input::KeyInputEvent> m_input_up_keys;

	double m_mouse_xpos{ 0 };
	double m_mouse_ypos{ 0 };
	double m_mouse_dx{ 0 };
	double m_mouse_dy{ 0 };
	double m_mouse_sensitivity{ 1 };
	int m_frames_mouse_moved{ 0 };



	//MouseButtonAction OnMouseMoveInput_delegate;
	//MouseButtonAction OnMouseButtonInput_delegate;

	// callbacks from window
	void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/);
	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/);

	void update(float dt);

	void update_keys(float dt);
	void update_mouse_move(float dt);
	void update_mouse_press(float dt);

	bool get_key_down(input::KeyCode key);
	bool get_key(input::KeyCode key);
	bool get_key_up(input::KeyCode key);
	bool get_mouse_key_down(input::MouseButton key);
	bool get_mouse_key(input::MouseButton key);
	bool get_mouse_key_up(input::MouseButton key);
	void set_mouse_visibility(bool visible);
	double get_input_x(std::string device);
	double get_input_y(std::string device);
	glm::vec2 get_mouse_pos();

};