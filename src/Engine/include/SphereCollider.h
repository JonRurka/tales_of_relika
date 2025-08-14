#pragma once

#include "Collider.h"

#define DEFAULT_SIZE (1.0f)

class SphereCollider : public Collider {
public:

	void Radius(float radius);

private:

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	std::unique_ptr<btCollisionShape> m_shape;
	std::unique_ptr<btDefaultMotionState> m_motionState;
#else
#endif

	inline static const std::string LOG_LOC{ "SPHERE_COLLIDER" };

protected:

	float m_radius{ DEFAULT_SIZE };

	void Init() override;
	void Update(float dt) override;
	void Load(json data) override;

	//void OnUpdateMass(float mass) override;
	void OnRefresh() override;
};

#undef DEFAULT_SIZE