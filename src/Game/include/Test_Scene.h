#pragma once

#include "game_engine.h"

class Standard_Material;

class Test_Scene  : public Scene{
public:


protected:
	void Init() override;

	void Update(float dt) override;

private:

	Standard_Material* standard_mat{nullptr};

	void create_light_object(WorldObject** obj, Light** light_comp, Light::Light_Type type, glm::vec3 pos, float scale, glm::vec4 color);



	const std::string LOG_LOC{ "TEST_SCENE" };
};
