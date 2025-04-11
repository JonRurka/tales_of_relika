#include "BoxCollider.h"

#include "WorldObject.h"
#include "Transform.h"

#define DEFAULT_SIZE (1.0f)

void BoxCollider::Init()
{
	base_Init();

	m_shape = new btBoxShape(btVector3(btScalar(DEFAULT_SIZE), btScalar(DEFAULT_SIZE), btScalar(DEFAULT_SIZE)));
	Physics::Add_Box_Shape(m_shape);
}

void BoxCollider::Update(float dt)
{
	base_Update(dt);


}

void BoxCollider::Load(json data)
{
}

void BoxCollider::OnUpdateMass(float mass)
{
	if (Is_Dynamic()) {
		m_shape->calculateLocalInertia(mass, m_localInertia);
	}
	else {
		m_localInertia = btVector3(0.0f, 0.0f, 0.0f);
	}

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(get_bt_transform());
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, m_shape, m_localInertia);
	set_rigidbody(new btRigidBody(rbInfo));
}
