#include "Engine.h"

#include "Graphics.h"
#include "Utilities.h"
#include "Logger.h"
#include "Scene.h"
#include "Resources.h"
#include "Light.h"
#include "Input.h"
#include "Physics.h"
#include "Camera.h"
#include "UI_Engine.h"

#include "tracy/Tracy.hpp"

#include <queue>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define MULTITHREADED_PHYSICS true

Engine* Engine::m_instance{nullptr};

void Engine::Activate_Scene(Scene& value)
{
	Activate_Scene(value.Name());
}

void Engine::Activate_Scene(std::weak_ptr<Scene> value)
{
	assert(!value.expired());
	Activate_Scene(value.lock()->Name());
}

void Engine::Activate_Scene(std::shared_ptr<Scene> value)
{
	Activate_Scene(value->Name());
}

void Engine::Activate_Scene(std::string value)
{
	assert(m_instance->m_scenes.contains(value));

	std::weak_ptr<Scene> scene_val = m_instance->m_scenes[value];
	//Activate_Scene(m_instance->m_scenes[value]);

	if (m_instance->m_has_active_scene && m_instance->m_active_scene.lock()->Name() == value)
		return;

	if (m_instance->m_has_active_scene) {
		//Logger::LogInfo(LOG_POS("Activate_Scene"), "Deactivating scene: %s", m_instance->m_active_scene->Name().c_str());
		m_instance->m_active_scene.lock()->Activate(false);

	}

	//Logger::LogInfo(LOG_POS("Activate_Scene"), "Activating scene: %s", m_instance->m_active_scene->Name().c_str());
	m_instance->m_active_scene = scene_val;
	m_instance->m_active_scene.lock()->Activate(true);
	m_instance->m_has_active_scene = true;
}

Scene& Engine::Active_Scene()
{
	assert(m_instance->m_has_active_scene);
	return *m_instance->m_active_scene.lock();
}

std::shared_ptr<Scene> Engine::Get_Scene_Ptr(std::string name)
{
	assert(m_instance->m_scenes.contains(name));
	return m_instance->m_scenes[name];
}

std::weak_ptr<Scene> Engine::Active_Scene_Ptr()
{
	assert(m_instance->m_has_active_scene);
	return m_instance->m_active_scene;
}

float Engine::Run_Time()
{
	return Utilities::Get_Time();// -m_instance->m_start_time;
}

int Engine::Run()
{
	initialize();
	game_loop();
	return 0;
}

void Engine::Stop()
{


	m_running = false;
}

void Engine::initialize_scene(std::shared_ptr<Scene> scene, std::string name)
{
	scene->m_name = name;
	m_scenes[name] = scene;
}

void Engine::Initialize_Scene(Scene* scene, json data)
{
	//std::string name;
	//data["name"].get_to(name);
	//m_scenes[name] = scene;


}

void Engine::initialize()
{
	Utilities::Init_Time();
	m_instance = this;
	m_running = true;
	m_start_time = Utilities::Get_Time();
	
	Resources::Instance().Init();
	Input::Instance().Init();
	Graphics::Instance().Init();
	Physics::Instance().Init(MULTITHREADED_PHYSICS);

	Graphics::Instance().Clear_Color(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

	Init();
}

void Engine::game_loop()
{
	std::vector<float> previous_frame_times;
	previous_frame_times.reserve(10);

	float lastFrame = (float)Utilities::Get_Time();
	while (m_running && !Graphics::Instance().Window_Should_Close())
	{
		ZoneScopedN("Client Main Loop");

		process_input();

		if (!MULTITHREADED_PHYSICS)
			Physics::Instance().update_internal(m_deltaTime);

		if (Graphics::Instance().Render_ImgUI()) {
			ZoneScopedN("ImgUI begin frame.");
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		if (m_has_active_scene)
		{
			Active_Scene().Update_internal(m_deltaTime);
		}

		Update(m_deltaTime);


		Light::Update_Lights(m_deltaTime);
		Graphics::Instance().Update(m_deltaTime);

		Logger::Update();

		ZoneNamedN(swapzone, "glfw Poll Events", true);
		glfwPollEvents();

		ZoneNamedN(endzone, "end frame", true);
		float newTime = (float)Utilities::Get_Time();
		m_deltaTime = newTime - lastFrame;
		lastFrame = newTime;

		previous_frame_times.push_back(m_deltaTime);
		if (previous_frame_times.size() > 10) {
			previous_frame_times.erase(previous_frame_times.begin());
		}
		float time_sum = 0;
		for (const auto& t : previous_frame_times) {
			time_sum += t;
		}
		m_avg_deltaTime = time_sum / previous_frame_times.size();
		m_fps = 1.0f / m_avg_deltaTime;

		// Tracy end frame.
		FrameMark;
	}

	cleanup();
}

void Engine::process_input()
{
	Input::Instance().update(m_deltaTime);
}

void Engine::cleanup()
{
	Logger::LogDebug(LOG_POS("cleanup"), "Shutting down...");

	if (m_instance->m_has_active_scene) {
		Logger::LogInfo(LOG_POS("cleanup"), "Deactivating scene: %s", m_instance->m_active_scene.lock()->Name().c_str());
		m_instance->m_active_scene.lock()->Activate(false);
	}

	Logger::LogInfo(LOG_POS("cleanup"), "UI Engine shutdown.");
	UI_Engine::Instance().Shutdown();

	Logger::LogInfo(LOG_POS("cleanup"), "Destroy camera.");
	Camera::StaticDestroy();
	
	Logger::LogInfo(LOG_POS("cleanup"), "Finished.");
}
