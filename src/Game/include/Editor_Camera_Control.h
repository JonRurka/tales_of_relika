#pragma once

#include "game_engine.h"

class Editor_Camera_Control : public Component
{
public:

protected:
	void Init() override;

	void Update(float dt) override;

private:

	Transform* trans{ nullptr };
	float m_horizontalAngle{ 0 };
	float m_verticalAngle{ 0 };
	glm::vec3 m_euler;
	float m_move_speed{ 1.0f };

	glm::vec3 m_control_point{glm::vec3(0.0, 0.0f, -1.0f)};

	void update_rotation(float dt, float mouse_x, float mouse_y);

	inline static const std::string LOG_LOC{ "EDITOR_CAM_CONTROL" };
};