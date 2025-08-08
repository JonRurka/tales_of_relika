#pragma once

#include "Collider.h"

#define DEFAULT_CAPSUE_RADIUS (0.5f)
#define DEFAULT_CAPSUE_HEIGHT (1.5f)

class btPairCachingGhostObject;
class btKinematicCharacterController;

class CharacterCollider : public Collider {
public:

	void Radius(float radius);

	void Height(float height);

	btPairCachingGhostObject& Get_Ghost_Object() { return *m_ghostObject.get(); }

	btKinematicCharacterController& Get_Controller() { return *m_charCon.get(); }

private:

	std::unique_ptr<btCapsuleShapeZ> m_shape{ nullptr };


	inline static const std::string LOG_LOC{ "CHARACTER_COLLIDER" };

protected:

	float m_radius{ DEFAULT_CAPSUE_RADIUS };
	float m_height{ DEFAULT_CAPSUE_HEIGHT };
	std::unique_ptr<btPairCachingGhostObject> m_ghostObject;
	std::unique_ptr<btKinematicCharacterController> m_charCon;

	void Init() override;
	void Update(float dt) override;
	void Load(json data) override;
	void OnDestroy() override;

	//void OnUpdateMass(float mass) override;
	void OnRefresh() override;
};

#undef DEFAULT_SIZE