#include "SphereCollider.h"

#include "WorldObject.h"
#include "Transform.h"

#define DEFAULT_SIZE (1.0f)

void SphereCollider::Init()
{
	base_Init();

	m_shape = new btSphereShape(m_radius);
}

void SphereCollider::Radius(float radius)
{
	if (m_shape != nullptr) {
		delete m_shape;
	}
	m_shape = new btSphereShape(m_radius);
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

	if (RigidBody() != nullptr) {
		remove_rigidbody(RigidBody());
	}

	if (Is_Dynamic()) {
		m_shape->calculateLocalInertia(Mass(), m_localInertia);
	}
	else {
		m_localInertia = btVector3(0.0f, 0.0f, 0.0f);
	}

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(create_bt_transform());
	btRigidBody::btRigidBodyConstructionInfo rbInfo(Mass(), myMotionState, m_shape, m_localInertia);
	set_rigidbody(new btRigidBody(rbInfo));
}