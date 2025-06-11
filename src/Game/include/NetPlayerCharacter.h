#pragma once

#include "game_engine.h"

#include "Network/OpCodes.h"
#include "Network/Data.h"

#include <vector>

class NetPlayerCharacter : public Component {
public:

	void Init(std::string userName, uint32_t user_id, glm::vec3 pos, glm::quat rot);

	void Set_Orientation(glm::vec3 loc, glm::quat rot);

	void Execute_Player_Event(OpCodes::Player_Events event_cmd, std::vector<uint8_t> data);

	void Jump_Player_Event();

	std::string Get_UserName() {
		return m_username;
	}

	uint32_t Get_UserID() {
		return m_user_id;
	}

protected:

	void Init() override;

	void Update(float dt) override;

private:

	bool m_is_jumping{ false };
	bool m_is_falling{ false };
	std::string m_username;
	uint32_t m_user_id;

	glm::vec3 m_velocity;

	glm::vec3 m_old_location; // last frame location
	glm::quat m_old_rotation; // loast frame rotation

	glm::vec3 m_new_location; // new location from server
	glm::quat m_new_rotation; // new rotation from server

	float m_syncDelay;
	float m_syncTime;
	double m_last_orientation_update;
	glm::vec3 m_current_location; // most recent exact location
	glm::quat m_current_rotation; // most recent exact location

	glm::vec3 m_sync_from_location; // location to start interpolation from
	glm::quat m_sync_from_rotation; // roration to start interpolation from

	const int max_velocity_entries = 5;
	std::vector<glm::vec3> m_velocity_smoothing_arr;

	bool VectorIsZero(glm::vec3 vec);
	glm::vec3 SmoothVelocity(glm::vec3 newVelocity);

	int64_t m_rand_seed;

	inline static const std::string LOG_LOC{ "NET_PLAYER_CHARACTER" };
};