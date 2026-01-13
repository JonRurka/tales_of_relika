#pragma once

#include "Collider.h"

#define DEFAULT_RADIUS (0.5f)
#define DEFAULT_HEIGHT (2.0f)

class CapsuleCollider : public Collider {
public:

	void Radius(float radius);

	void Height(float height);

private:

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	std::unique_ptr<btCollisionShape> m_shape;
	std::unique_ptr<btDefaultMotionState> m_motionState;
#else
	Ref<CapsuleShape> m_shape;
	Ref<Body> m_rigidbody;
#endif


	inline static const std::string LOG_LOC{ "CAPSULE_COLLIDER" };

protected:

	float m_radius{ DEFAULT_RADIUS };
	float m_height{ DEFAULT_HEIGHT };

	void Init() override;
	void Update(float dt) override;
	void FixedUpdate(float dt) override;
	void Load(json data) override;
	void OnDestroy() override;

	//void OnUpdateMass(float mass) override;
	void OnRefresh() override;
};

