#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Scene;
class Graphics;
class Resources;

class Engine {
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

	Scene* m_active_scene{ nullptr };
	Graphics* m_graphics{nullptr};

	Resources* m_resources{ nullptr };

	float m_start_time{ 0 };

	static Engine* m_instance;

	void Initialize_Scene(Scene* scene, std::string name);
	void Initialize_Scene(Scene* scene, json data);

	void initialize();

	void game_loop();

	const std::string LOG_LOC{ "ENGINE" };
};