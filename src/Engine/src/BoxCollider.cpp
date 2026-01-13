#include "BoxCollider.h"

#include "WorldObject.h"
#include "Transform.h"
#include "Logger.h"
#include "Graphics.h"

#define DEFAULT_SIZE (1.0f)

void BoxCollider::Init()
{
	base_Init();

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	m_shape = std::make_unique<btBoxShape>(btVector3(btScalar(m_size.x), btScalar(m_size.y), btScalar(m_size.z)));
#else
	m_shape = new BoxShape(Vec3(m_size.x, m_size.y, m_size.y), 0.0f);
#endif
}

void BoxCollider::Size(glm::vec3 size)
{
	m_size = size;
	//Logger::LogDebug(LOG_POS("Size"), "Set box size to: (%f, %f, %f)",
	//	size.x, size.y, size.z);

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	m_shape.reset();
	m_shape = std::make_unique<btBoxShape>(btVector3(btScalar(m_size.x), btScalar(m_size.y), btScalar(m_size.z)));
#else
	m_shape = new BoxShape(Vec3(m_size.x, m_size.y, m_size.z), 0.0f);
#endif

	OnRefresh();
}

void BoxCollider::Update(float dt)
{
	base_Update(dt);

	/*
	AABox box = Rigidbody().GetWorldSpaceBounds();
	box.GetSize();
	glm::vec3 min = glm::vec3(box.mMin.GetX(), box.mMin.GetY(), box.mMin.GetZ());
	glm::vec3 max = glm::vec3(box.mMax.GetX(), box.mMax.GetY(), box.mMax.GetZ());
	glm::vec3 extent = glm::vec3(box.GetSize().GetX(), box.GetSize().GetY(), box.GetSize().GetZ());

	Graphics::DrawDebugRay(min, glm::vec3(0, 1, 0), glm::vec3(1, 1, 0));
	Graphics::DrawDebugRay(max, glm::vec3(0, 1, 0), glm::vec3(1, 1, 0));
	Graphics::DrawDebugLine(min, max, glm::vec3(1, 0, 0));*/



	//Logger::LogDebug(LOG_POS("Size"), "Box size: (%f, %f, %f)",
	//	extent.x, extent.y, extent.z);
}

void BoxCollider::Load(json data)
{
}

void BoxCollider::OnRefresh()
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
	//Logger::LogDebug(LOG_POS("OnRefresh"), "Created rigidbody.");
#else

	ObjectLayer layer = Layers::MOVING;
	EMotionType e_type = EMotionType::Dynamic;

	if (!Is_Dynamic()) {
		layer = Layers::NON_MOVING;
		e_type = EMotionType::Static;
	}

	glm::vec3 pos = Object().Get_Transform().Position();
	glm::quat rot = Object().Get_Transform().Rotation();

	create_Rigidbody(BodyCreationSettings(m_shape, RVec3(pos.x, pos.y, pos.z), Quat(rot.x, rot.y, rot.z, rot.w), e_type, layer));
	//Body* rigidbody = Physics::GetBodyInterface().CreateBody(BodyCreationSettings(m_shape, RVec3(pos.x, pos.y, pos.z), Quat(rot.x, rot.y, rot.z, rot.w), e_type, layer));
	//Physics::GetBodyInterface().AddBody(rigidbody->GetID(), EActivation::Activate);
	//set_rigidbody(rigidbody);
#endif
}

void BoxCollider::OnDestroy()
{
	Destroy_Collider();
}
