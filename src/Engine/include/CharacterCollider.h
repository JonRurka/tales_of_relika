#pragma once

#include "Collider.h"

#define DEFAULT_RADIUS (0.5f)
#define DEFAULT_HEIGHT (1.5f)

class btPairCachingGhostObject;
class btKinematicCharacterController;

class CharacterCollider : public Collider {
public:

	void Radius(float radius);

	void Height(float height);

	btPairCachingGhostObject* Get_Ghost_Object() { return m_ghostObject; }

	btKinematicCharacterController* Get_Controller() { return m_charCon; }

private:

	btCollisionShape* m_shape{ nullptr };


	inline static const std::string LOG_LOC{ "CHARACTER_COLLIDER" };

protected:

	float m_radius{ DEFAULT_RADIUS };
	float m_height{ DEFAULT_HEIGHT };
	btPairCachingGhostObject* m_ghostObject;
	btKinematicCharacterController* m_charCon;

	void Init() override;
	void Update(float dt) override;
	void Load(json data) override;
	void OnDestroy() override;

	//void OnUpdateMass(float mass) override;
	void OnRefresh() override;
};

#undef DEFAULT_SIZE