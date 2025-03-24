#include "Engine.h"

#include "Graphics.h"
#include "Utilities.h"
#include "Logger.h"
#include "Scene.h"
#include "Resources.h"
#include "Light.h"

Engine* Engine::m_instance{nullptr};

void Engine::Activate_Scene(Scene* value)
{
	if (m_instance->m_active_scene == value)
		return;

	if (m_instance->m_active_scene != nullptr) {
		m_instance->m_active_scene->Activate(false);
	}
	m_instance->m_active_scene = value;
	m_instance->m_active_scene->Activate(true);
}

void Engine::Activate_Scene(std::string value)
{
	Activate_Scene(m_instance->m_scenes[value]);
}

float Engine::Run_Time()
{
	return Utilities::Get_Time() - m_instance->m_start_time;
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

void Engine::Initialize_Scene(Scene* scene, std::string name)
{
	m_scenes[name] = scene;
	

}

void Engine::Initialize_Scene(Scene* scene, json data)
{
	std::string name;
	data["name"].get_to(name);
	m_scenes[name] = scene;


}

void Engine::initialize()
{
	m_instance = this;
	m_running = true;
	m_start_time = Utilities::Get_Time();
	Init_Resources<Resources>();

	m_graphics = new Graphics();
	m_graphics->Initialize();
	m_graphics->Clear_Color(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

	Init();
}

void Engine::game_loop()
{

	float lastFrame = (float)Utilities::Get_Time();
	while (m_running && !m_graphics->Window_Should_Close())
	{

		if (Active_Scene() != nullptr)
		{
			Active_Scene()->Update_internal(m_deltaTime);
		}

		Update(m_deltaTime);


		Light::Update_Lights(m_deltaTime);
		m_graphics->Update(m_deltaTime);
		Logger::Update();

		glfwPollEvents();

		float newTime = (float)Utilities::Get_Time();
		m_deltaTime = newTime - lastFrame;
		lastFrame = newTime;
	}
}
