#include "Menu_Scene.h"

#include "Game_Resources.h"
#include "Game.h"

#include <RmlUi/Core.h>

void Menu_Scene::Init()
{
	setup_camera();

	m_startup_splash = UI_Engine::Instance()->Load_Document_Resource("splash", Game_Resources::UI::Documents::MENU::SPLASH);
	m_main_menu_screen = UI_Engine::Instance()->Load_Document_Resource("main_menu", Game_Resources::UI::Documents::MENU::MAIN);

	m_start_sp_button_elem = m_main_menu_screen->GetElementById("start_sp_button");

	add_main_menu_listeners();

	m_main_menu_screen->Show();

	UI_Engine::Instance()->Accept_Input(true);

	m_started_time = Utilities::Get_Time();

}

void Menu_Scene::Update(float dt)
{
	if (!m_game_started && (Utilities::Get_Time() - m_started_time > 1.0f))
	{
		m_game_started = true;
		OnStartSP_Clicked();
	}
}

void Menu_Scene::Deactivate()
{
	//remove_main_menu_listeners();
}


void Menu_Scene::OnStartSP_Clicked()
{
	Logger::LogDebug(LOG_POS("OnStartSP_Clicked"), "Clicked start SP game.");
	m_main_menu_screen->Hide();
	UI_Engine::Instance()->Accept_Input(false);
	m_camera->Activate(false);
	Game::OpenVoxelWorld();
}

void Menu_Scene::add_main_menu_listeners()
{
	m_start_sp_event_listener = new Start_SP_Click_Event(this);
	m_start_sp_button_elem->AddEventListener(Rml::EventId::Click, m_start_sp_event_listener);
}

void Menu_Scene::remove_main_menu_listeners()
{
	m_start_sp_button_elem->RemoveEventListener(Rml::EventId::Click, m_start_sp_event_listener);
	delete m_start_sp_event_listener;
	m_start_sp_event_listener = nullptr;
}

void Menu_Scene::setup_camera()
{
	m_camera_obj = Instantiate("camera");
	//Camera_obj->Get_Transform()->Position(glm::vec3(0, 5, 6));
	m_camera_obj->Get_Transform()->Position(glm::vec3(0, 10, -50));
	m_camera_obj->Get_Transform()->LookAt(glm::vec3(0.0f, 10.0f, 100.0f));
	m_camera = m_camera_obj->Add_Component<Camera>();
	m_camera->Clear_Color(glm::vec4(0.0, 0.0, 0.0, 1.0));
}