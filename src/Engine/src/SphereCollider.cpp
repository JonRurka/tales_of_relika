#include "SphereCollider.h"

#include "WorldObject.h"
#include "Transform.h"

#define DEFAULT_SIZE (1.0f)

void SphereCollider::Init()
{
	base_Init();

	m_shape = std::make_unique<btSphereShape>(m_radius);
}

void SphereCollider::Radius(float radius)
{
	m_shape.reset();
	m_radius = radius;
	m_shape = std::make_unique<btSphereShape>(m_radius);
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
	btDefaultMotionState* myMotionState = new btDefaultMotionState(create_bt_transform());
	btRigidBody::btRigidBodyConstructionInfo rbInfo(Mass(), myMotionState, m_shape.get(), m_localInertia);
	set_rigidbody(std::make_shared<btRigidBody>(rbInfo));
}