#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#include <nlohmann/json.hpp>

#include "input_events.h"

using json = nlohmann::json;

class Scene;
class Graphics;
class Resources;
class window;
class Input;
class Physics;
struct GLFWwindow;

class Engine {
	friend class window;
	friend class Graphics;
public:
	
	static Engine* Instance() { return m_instance; }

	static bool Running() { return m_instance->m_running; }
	
	static Scene& Active_Scene();
	static std::weak_ptr<Scene> Active_Scene_Ptr();

	static std::shared_ptr<Scene> Get_Scene_Ptr(std::string name);

	static void Activate_Scene(Scene& value);
	static void Activate_Scene(std::weak_ptr<Scene> value);
	static void Activate_Scene(std::shared_ptr<Scene> value);
	static void Activate_Scene(std::string value);

	static float Run_Time();

	static float FPS() { return m_instance->m_fps; }


	template<typename T,
		typename = std::enable_if_t<std::is_base_of<Scene, T>::value>>
		std::shared_ptr<Scene> Load_Scene(std::string name)
	{
		std::shared_ptr<T> scene = std::make_shared<T>();
		scene->m_name = name;
		m_scenes[name] = scene;
		//initialize_scene(std::static_pointer_cast<Scene>(scene), name);
		//Initialize_Scene(static_cast<Scene*>(scene), name);
		return std::static_pointer_cast<Scene>(scene);
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
	std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;
	bool m_running{ false };
	float m_deltaTime{ 0 };
	float m_avg_deltaTime{ 0 };
	float m_fps{ 0 };

	
	std::weak_ptr<Scene> m_active_scene;
	bool m_has_active_scene{ false };

	float m_start_time{ 0 };

	static Engine* m_instance;

	void initialize_scene(std::shared_ptr<Scene> scene, std::string name);

	void Initialize_Scene(Scene* scene, json data);

	void initialize();

	void game_loop();

	void process_input();

	void cleanup();

	inline static const std::string LOG_LOC{ "ENGINE" };
};