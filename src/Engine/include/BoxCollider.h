#pragma once

#include "Collider.h"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>

#define DEFAULT_SIZE (1.0f)

class BoxCollider : public Collider {
public:
	typedef std::shared_ptr<BoxCollider> Shared;
	typedef std::weak_ptr<BoxCollider> Weak;

	void Size(glm::vec3 size);

private:

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	std::unique_ptr<btCollisionShape> m_shape;
	std::unique_ptr<btDefaultMotionState> m_motionState;
#else
	Ref<BoxShape> m_shape;

#endif

	inline static const std::string LOG_LOC{ "BOX_COLLIDER" };

protected:

	glm::vec3 m_size {glm::vec3(DEFAULT_SIZE, DEFAULT_SIZE, DEFAULT_SIZE)};

	void Init() override;
	void Update(float dt) override;
	void Load(json data) override;

	//void OnUpdateMass(float mass) override;
	void OnRefresh() override;

	void OnDestroy() override;
};

#undef DEFAULT_SIZE