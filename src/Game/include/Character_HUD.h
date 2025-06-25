#pragma once

#include "game_engine.h"

class Character_HUD : public Component {

public:

	void Init(Camera* camera);

protected:
	void Init() override;

	void Update(float dt) override;

private:

	Camera* m_camera{ nullptr };

	void draw_voxel_box(glm::ivec3 voxel_coord);

	inline static const std::string LOG_LOC{ "CHARACTER_HUD" };

};