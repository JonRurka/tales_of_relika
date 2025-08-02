#include "Menu_Scene.h"

#include "Game_Resources.h"

#include <RmlUi/Core.h>

void Menu_Scene::Init()
{
	setup_camera();

	m_startup_splash = UI_Engine::Instance()->Load_Document_Resource("hot_bar", Game_Resources::UI::Documents::MENU::SPLASH);
	m_main_menu_screen = UI_Engine::Instance()->Load_Document_Resource("hot_bar", Game_Resources::UI::Documents::MENU::MAIN);

	m_startup_splash->Show();



}

void Menu_Scene::Update(float dt)
{

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