#pragma once

#include "Collider.h"

#define DEFAULT_RADIUS (0.5f)
#define DEFAULT_HEIGHT (2.0f)

class CapsuleCollider : public Collider {
public:

	void Radius(float radius);

	void Height(float height);

private:

	btCollisionShape* m_shape{ nullptr };


	inline static const std::string LOG_LOC{ "CAPSULE_COLLIDER" };

protected:

	float m_radius{ DEFAULT_RADIUS };
	float m_height{ DEFAULT_HEIGHT };

	void Init() override;
	void Update(float dt) override;
	void Load(json data) override;

	//void OnUpdateMass(float mass) override;
	void OnRefresh() override;
};

#undef DEFAULT_SIZE