#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include <nlohmann/json.hpp>

#include "input_events.h"

using json = nlohmann::json;

class Scene;
class Graphics;
class Resources;
class window;
struct GLFWwindow;

class Engine {
	friend class window;
	friend class Graphics;
public:
	
	static Engine* Instance() { return m_instance; }

	static bool Running() { return m_instance->m_running; }

	static Scene* Active_Scene() { return m_instance->m_active_scene; }
	static void Activate_Scene(Scene* value);
	static void Activate_Scene(std::string value);

	static float Run_Time();

	static float FPS() { return m_instance->m_fps; }

	static Graphics* graphics() { return m_instance->m_graphics; }

	static Resources* resources() { return m_instance->m_resources;}

	static bool GetKeyDown(Input::KeyCode key) {
		return m_instance->get_key_down(key);

	}
	static bool GetKey(Input::KeyCode key) {
		return m_instance->get_key(key);
	}
	static bool GetKeyUp(Input::KeyCode key) {
		return m_instance->get_key_up(key);
	}
	static double Get_Input_X() { return m_instance->get_input_x(); }
	static double Get_Input_Y() { return m_instance->get_input_y(); }

	static void Mouse_Sensitivity(double value) { m_instance->m_mouse_sensitivity = value; }


	template<typename T,
		typename = std::enable_if_t<std::is_base_of<Resources, T>::value>>
		T * Init_Resources()
	{
		T* res = new T();
		m_resources = static_cast<Resources*>(res);
		return res;
	}


	template<typename T,
		typename = std::enable_if_t<std::is_base_of<Scene, T>::value>>
		T * Load_Scene(std::string name)
	{
		T* scene = new T();
		//Initialize_Scene(static_cast<Scene*>(scene), name);
		return scene;
	}

	/*template<typename T,
		typename = std::enable_if_t<std::is_base_of<Scene, T>::value>>
		T * Load_Scene(json data)
	{
		T* scene = new T();
		Initialize_Scene(static_cast<Scene*>(scene), data);
		return scene;
	}*/

	int Run();

	void Stop();


	
protected:

	virtual void Init() = 0;

	virtual void Update(float dt) = 0;

private:
	std::unordered_map<std::string, Scene*> m_scenes;
	bool m_running{ false };
	float m_deltaTime{ 0 };
	float m_avg_deltaTime{ 0 };
	float m_fps{ 0 };

	std::unordered_map<int, Input::KeyCode> m_key_lookup;

	Scene* m_active_scene{ nullptr };
	Graphics* m_graphics{nullptr};

	Resources* m_resources{ nullptr };

	float m_start_time{ 0 };

	static Engine* m_instance;

	struct KeyInputAction {
		Input::OnKeyInputActionPtr action;
		void* pointer{ nullptr };
		bool set{ false };

		void Call(Input::KeyInputEvent key_event) {
			if (set) {
				action(pointer, key_event);
			}
		}
	} OnKeyInput_delegate;

	struct MouseButtonAction {
		Input::OnMouseButtonInputActionPtr action;
		void* pointer{ nullptr };
		bool set{ false };

		void Call(Input::MouseButtonInputEvent mouse_event) {
			if (set) {
				action(pointer, mouse_event);
			}
		}
	};

	std::unordered_map<Input::KeyCode, Input::KeyInputEvent> m_input_down_keys;
	std::unordered_map<Input::KeyCode, Input::KeyInputEvent> m_input_pressed_keys;
	std::unordered_map<Input::KeyCode, Input::KeyInputEvent> m_input_up_keys;
	double m_mouse_xpos{ 0 };
	double m_mouse_ypos{ 0 };
	double m_mouse_dx{ 0 };
	double m_mouse_dy{ 0 };
	double m_mouse_sensitivity{ 1 };
	int m_frames_mouse_moved{ 0 };

	MouseButtonAction OnMouseMoveInput_delegate;
	MouseButtonAction OnMouseButtonInput_delegate;

	// callbacks from window
	void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/);
	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/);


	void Initialize_Scene(Scene* scene, std::string name);
	void Initialize_Scene(Scene* scene, json data);

	void initialize();

	void game_loop();

	void process_input();

	bool get_key_down(Input::KeyCode key);
	bool get_key(Input::KeyCode key);
	bool get_key_up(Input::KeyCode key);
	double get_input_x();
	double get_input_y();

	const std::string LOG_LOC{ "ENGINE" };
};