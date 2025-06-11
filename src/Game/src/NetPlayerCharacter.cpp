#include "NetPlayerCharacter.h"

#include <math.h>
#include <algorithm>

void NetPlayerCharacter::Init()
{
	m_syncDelay = 0.01;
	m_syncTime = 0.01;
	m_last_orientation_update = Utilities::Get_Time();
}

void NetPlayerCharacter::Update(float dt)
{
	m_syncTime += dt;

	float delta = std::clamp(m_syncTime / m_syncDelay, 0.0f, 1.0f);
	m_current_location = glm::mix(m_sync_from_location, m_new_location, delta);
	m_current_rotation = glm::slerp(m_sync_from_rotation, m_new_rotation, delta);

	Object()->Get_Transform()->Position(m_current_location);
	Object()->Get_Transform()->Rotation(m_current_rotation);





	m_old_location = m_current_location;
	m_old_rotation = m_current_rotation;

}

void NetPlayerCharacter::Init(std::string userName, uint32_t user_id, glm::vec3 pos, glm::quat rot)
{
	m_username = userName;
	m_user_id = user_id;
	m_current_location = pos;
	m_current_rotation = rot;
	m_old_location = pos;
	m_old_rotation = rot;
	Object()->Get_Transform()->Position(pos);
	Object()->Get_Transform()->Rotation(rot);
}

void NetPlayerCharacter::Set_Orientation(glm::vec3 loc, glm::quat rot)
{
	double now = Utilities::Get_Time();
	m_syncTime = 0;
	m_new_location = loc;
	m_new_rotation = rot;
	m_sync_from_location = m_current_location;
	m_sync_from_rotation = m_current_rotation;
	m_syncDelay = (now - m_last_orientation_update);
	m_last_orientation_update = now;
}

void NetPlayerCharacter::Execute_Player_Event(OpCodes::Player_Events event_cmd, std::vector<uint8_t> data)
{
	switch (event_cmd) {
	case OpCodes::Player_Events::Jump:
		Jump_Player_Event();
		break;
	}
}

void NetPlayerCharacter::Jump_Player_Event()
{
	m_is_jumping = true;
}

bool NetPlayerCharacter::VectorIsZero(glm::vec3 vec)
{
	return glm::distance(vec, glm::vec3(0.0)) < 0.001;
}

glm::vec3 NetPlayerCharacter::SmoothVelocity(glm::vec3 newVelocity)
{



	return glm::vec3();
}
