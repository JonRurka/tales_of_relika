#include "Collider.h"

#include "WorldObject.h"
#include "Transform.h"

void Collider::base_Init()
{
}

void Collider::base_Update(float dt)
{
	Transform* obj_trans = Object()->Get_Transform();
	btTransform bt_trans;
	if (m_rigidbody && m_rigidbody->getMotionState())
	{
		m_rigidbody->getMotionState()->getWorldTransform(bt_trans);
	}
	else
	{
		bt_trans = m_rigidbody->getWorldTransform();
	}
	btVector3 pos = bt_trans.getOrigin();
	btQuaternion rot = bt_trans.getRotation();
	obj_trans->Position(glm::vec3(pos.x(), pos.y(), pos.z()));
	obj_trans->Rotation(glm::quat(rot.x(), rot.y(), rot.z(), rot.w()));
}

btTransform Collider::get_bt_transform()
{
	Transform* trans = Object()->Get_Transform();
	glm::vec3 pos = trans->Position();
	glm::quat rot = trans->Rotation();

	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

	return startTransform;
}

void Collider::set_rigidbody(btRigidBody* body)
{
	if (m_active) {
		m_rigidbody = body;
		Physics::Add_Rigidbody(m_rigidbody);
	}
}

void Collider::remove_rigidbody(btRigidBody* body)
{
	if (m_active && m_rigidbody != nullptr) {
		Physics::Remove_Rigidbody(body);
		m_rigidbody = nullptr;
	}
}
