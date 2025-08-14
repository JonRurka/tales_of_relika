#include "SphereCollider.h"

#include "WorldObject.h"
#include "Transform.h"

#define DEFAULT_SIZE (1.0f)

void SphereCollider::Init()
{
	base_Init();

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	m_shape = std::make_unique<btSphereShape>(m_radius);
#else
#endif
}

void SphereCollider::Radius(float radius)
{
	m_radius = radius;

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	m_shape.reset();
	m_shape = std::make_unique<btSphereShape>(m_radius);
#else
#endif

	OnRefresh();
}

void SphereCollider::Update(float dt)
{
	base_Update(dt);
}

void SphereCollider::Load(json data)
{
}

void SphereCollider::OnRefresh()
{
	if (!Active())
		return;

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	if (Has_Rigidbody()) {
		remove_rigidbody();
	}

	if (Is_Dynamic()) {
		m_shape->calculateLocalInertia(Mass(), m_localInertia);
	}
	else {
		m_localInertia = btVector3(0.0f, 0.0f, 0.0f);
	}

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	m_motionState = std::make_unique<btDefaultMotionState>(create_bt_transform());
	btRigidBody::btRigidBodyConstructionInfo rbInfo(Mass(), m_motionState.get(), m_shape.get(), m_localInertia);
	set_rigidbody(std::make_shared<btRigidBody>(rbInfo));
#else
#endif
}