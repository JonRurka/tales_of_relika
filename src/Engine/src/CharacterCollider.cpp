#include "CharacterCollider.h"

#include "WorldObject.h"
#include "Transform.h"
#include "Logger.h"

#include"BulletCollision/CollisionDispatch/btGhostObject.h"
#include"BulletDynamics/Character/btKinematicCharacterController.h"

void CharacterCollider::Init()
{
	base_Init();

	m_shape = new btCapsuleShape(m_radius, m_height);
}

void CharacterCollider::Radius(float radius) 
{
	if (m_shape != nullptr) {
		delete m_shape;
	}
	m_radius = radius;
	m_shape = new btCapsuleShape(m_radius, m_height);
}

void CharacterCollider::Height(float height) 
{
	if (m_shape != nullptr) {
		delete m_shape;
	}
	m_height = height;
	m_shape = new btCapsuleShape(m_radius, m_height);
}

void CharacterCollider::Update(float dt)
{
	if (m_charCon == nullptr)
		return;

	btTransform t;
	t = m_charCon->getGhostObject()->getWorldTransform();
	btVector3 pos = t.getOrigin();
	btQuaternion  quat = t.getRotation();

	Object()->Get_Transform()->Position(glm::fvec3(pos.x(), pos.y(), pos.z()));
	Object()->Get_Transform()->Rotation(glm::quat(quat.x(), quat.y(), quat.z(), quat.w()));

	Logger::LogDebug(LOG_POS("Update"), "(%f, %f, %f)",
		pos.x(), pos.y(), pos.z());
}

void CharacterCollider::Load(json data)
{
}

void CharacterCollider::OnDestroy()
{
}

void CharacterCollider::OnRefresh()
{
	if (!Active())
		return;

	//if (RigidBody() != nullptr) {
	//	remove_rigidbody(RigidBody());
	//}

	if (Is_Dynamic()) {
		m_shape->calculateLocalInertia(Mass(), m_localInertia);
	}
	else {
		m_localInertia = btVector3(0.0f, 0.0f, 0.0f);
	}

	m_ghostObject = new btPairCachingGhostObject();
	m_ghostObject->setWorldTransform(create_bt_transform());
	Physics::Get_Broadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	m_ghostObject->setCollisionShape(m_shape);
	m_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	m_charCon = new btKinematicCharacterController(m_ghostObject, (btCapsuleShape*)m_shape, 0.05f);
	m_charCon->setGravity(btVector3(0, Physics::Gravity(), 0));

	Physics::GetDynamicWorld()->addCollisionObject(m_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter);
	Physics::GetDynamicWorld()->addAction(m_charCon);

	Logger::LogDebug(LOG_POS("OnRefresh"), "Created character collider components.");
}


