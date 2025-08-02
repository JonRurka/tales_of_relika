#pragma once

#include "game_engine.h"

namespace Rml {
	class Context;
	class ElementDocument;
}

class Menu_Scene : public Scene {
public:


protected:
	void Init() override;

	void Update(float dt) override;

private:

	Rml::ElementDocument* m_startup_splash{ nullptr };
	Rml::ElementDocument* m_main_menu_screen{ nullptr };

	WorldObject* m_camera_obj{ nullptr };
	Camera* m_camera{ nullptr };

	void setup_camera();

	inline static const std::string LOG_LOC{ "MENU_SCENE" };
};
