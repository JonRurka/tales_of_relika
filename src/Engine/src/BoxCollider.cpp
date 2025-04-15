#include "BoxCollider.h"

#include "WorldObject.h"
#include "Transform.h"
#include "Logger.h"

#define DEFAULT_SIZE (1.0f)

void BoxCollider::Init()
{
	base_Init();

	m_shape = new btBoxShape(btVector3(btScalar(m_size.x), btScalar(m_size.y), btScalar(m_size.z)));
}

void BoxCollider::Size(glm::vec3 size)
{
	if (m_shape != nullptr) {
		delete m_shape;
	}
	m_size = size;
	m_shape = new btBoxShape(btVector3(btScalar(m_size.x), btScalar(m_size.y), btScalar(m_size.z)));
	OnRefresh();
}

void BoxCollider::Update(float dt)
{
	base_Update(dt);
}

void BoxCollider::Load(json data)
{
}

void BoxCollider::OnRefresh()
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
	Logger::LogDebug(LOG_POS("OnRefresh"), "Created rigidbody.");
}
