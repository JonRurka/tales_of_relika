#include "CapsuleCollider.h"

#include "WorldObject.h"
#include "Transform.h"

#define DEFAULT_SIZE (1.0f)

void CapsuleCollider::Init()
{
	base_Init();

	m_shape = new btSphereShape(m_radius);
	m_shape = new btCapsuleShape(m_radius, m_height);
}

void CapsuleCollider::Radius(float radius)
{
	if (m_shape != nullptr) {
		delete m_shape;
	}
	m_radius = radius;
	m_shape = new btSphereShape(m_radius);
	OnRefresh();
}

void CapsuleCollider::Height(float height)
{
	if (m_shape != nullptr) {
		delete m_shape;
	}
	m_height = height;
	m_shape = new btCapsuleShape(m_radius, m_height);
	OnRefresh();
}

void CapsuleCollider::Update(float dt)
{
	base_Update(dt);
}

void CapsuleCollider::Load(json data)
{
}

void CapsuleCollider::OnRefresh()
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