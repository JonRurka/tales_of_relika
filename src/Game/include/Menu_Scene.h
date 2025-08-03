#pragma once

#include "game_engine.h"

#include <RmlUi/Core.h>
#include "UI_Engine.h"


namespace Rml {
	class Context;
	class ElementDocument;
}

class Menu_Scene : public Scene {
public:


protected:
	void Init() override;

	void Update(float dt) override;

	void Deactivate() override;

private:

	DEFINE_UI_EVENT(Start_SP_Click_Event, Menu_Scene, OnStartSP_Clicked)

	Rml::ElementDocument* m_startup_splash{ nullptr };
	Rml::ElementDocument* m_main_menu_screen{ nullptr };

	Rml::Element* m_start_sp_button_elem{ nullptr };
	Start_SP_Click_Event* m_start_sp_event_listener{ nullptr };

	WorldObject* m_camera_obj{ nullptr };
	Camera* m_camera{ nullptr };

	double m_started_time{ 0 };
	bool m_game_started{ false };

	void OnStartSP_Clicked();

	void add_main_menu_listeners();
	void remove_main_menu_listeners();

	void setup_camera();

	inline static const std::string LOG_LOC{ "MENU_SCENE" };
};
