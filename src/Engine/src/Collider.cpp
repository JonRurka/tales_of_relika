#include "Collider.h"

#include "WorldObject.h"
#include "Transform.h"
#include "Logger.h"


void Collider::base_Init()
{
}

void Collider::base_Update(float dt)
{
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	if (!m_active || m_rigidbody == nullptr) {
		return;
	}

	if (Is_Dynamic()) {
		Transform& obj_trans = Object().Get_Transform();
		btTransform bt_trans = get_bt_rigid_transform();

		btVector3 pos = bt_trans.getOrigin();
		btQuaternion rot = bt_trans.getRotation();

		obj_trans.Position(glm::vec3(pos.x(), pos.y(), pos.z()));
		obj_trans.Rotation(glm::quat(rot.x(), rot.y(), rot.z(), rot.w()));

		//Logger::LogDebug(LOG_POS("base_Update"), "'%s' (%f): Updated Position: (%f, %f, %f)", 
		//	Object()->Name().c_str(), m_mass, pos.x(), pos.y(), pos.z());
	}
#else
	if (Is_Dynamic())
	{
		Transform& obj_trans = Object().Get_Transform();
		Vec3 pos = m_rigidbody->GetPosition();
		Quat rot = m_rigidbody->GetRotation();
		obj_trans.Position(glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ()));
		obj_trans.Rotation(glm::quat(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ()));
	}


#endif
}

void Collider::Destroy_Collider()
{
	if (Has_Rigidbody()) {
		//Logger::LogDebug(LOG_POS("Destroy_Collider"), "Remove rigidbody");
		remove_rigidbody();
	}

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	
#else

#endif
}


#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
btTransform Collider::create_bt_transform()
{
	Transform& trans = Object().Get_Transform();
	glm::vec3 pos = trans.Position();
	glm::quat rot = trans.Rotation();

	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

	//Logger::LogDebug(LOG_POS("get_bt_transform"), "Reported pos: (%f, %f, %f)", pos.x, pos.y, pos.z);

	return startTransform;
}

btTransform Collider::get_bt_rigid_transform()
{
	btTransform bt_trans;

	if (!m_active || m_rigidbody == nullptr) {
		return bt_trans;
	}

	if (m_rigidbody && m_rigidbody->getMotionState())
	{
		m_rigidbody->getMotionState()->getWorldTransform(bt_trans);
	}
	else
	{
		bt_trans = m_rigidbody->getWorldTransform();
	}
	return bt_trans;
}

void Collider::set_rigidbody(std::shared_ptr<btRigidBody> body)
{
	if (m_active) {
		m_rigidbody = body;
		Physics::Add_Rigidbody(m_rigidbody.get());
		m_rigidbody->setUserPointer(this);
		//Logger::LogDebug(LOG_POS("set_rigidbody"), "Added regidbody.");
	}
}

void Collider::remove_rigidbody()
{
	if (m_active && m_rigidbody.get() != nullptr) {
		Physics::Remove_Rigidbody(m_rigidbody.get());
		m_rigidbody.reset();
	}
}
#else
void Collider::set_rigidbody(Body* body)
{
	m_rigidbody = body;
	Physics::Instance().Add_Rigidbody(m_rigidbody);
}

void Collider::remove_rigidbody()
{
	//Physics::GetBodyInterface().RemoveBody(m_rigidbody->GetID());
	Physics::Instance().Remove_Rigidbody(m_rigidbody);
	m_rigidbody = nullptr;
}

#endif

