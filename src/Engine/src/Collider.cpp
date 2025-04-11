#include "Collider.h"

#include "WorldObject.h"
#include "Transform.h"

void Collider::base_Init()
{
}

void Collider::base_Update(float dt)
{
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
	m_rigidbody = body;
	Physics::Add_Rigidbody(m_rigidbody);
}
