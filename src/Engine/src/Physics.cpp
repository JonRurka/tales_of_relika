#include "Physics.h"

Physics* Physics::m_instance{nullptr};

namespace {
	btVector3 to_bt_vector(glm::vec3 value) {
		return btVector3(value.x, value.y, value.z);
	}

	glm::vec3 to_glm_vector(btVector3 value) {
		return glm::vec3(value.x(), value.y(), value.z());
	}
}

Physics::Physics() 
{
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	m_collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	m_overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	m_solver = new btSequentialImpulseConstraintSolver;

	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);
	m_dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

void Physics::update_internal(float dt)
{
	m_dynamicsWorld->updateAabbs();
	m_dynamicsWorld->computeOverlappingPairs();
	m_dynamicsWorld->stepSimulation(1.f / 60.f, 10);
}

void Physics::add_box_shape(btCollisionShape* shape)
{
	m_collisionShapes_box.push_back(shape);
}

void Physics::add_rigidbody(btRigidBody* body)
{
	m_dynamicsWorld->addRigidBody(body);
}

void Physics::remove_rigidbody(btRigidBody* body)
{
	btCollisionObject* obj = (btCollisionObject*)body;
	if (body && body->getMotionState())
	{
		delete body->getMotionState();
	}
	m_dynamicsWorld->removeCollisionObject(body);
	delete body;
}


void Physics::Init()
{
	m_instance = new Physics();
}


Physics::RayHit Physics::Raycast(glm::vec3 start, glm::vec3 dir)
{
	RayHit res;
	res.did_hit = false;
	res.start = start;

	if (m_instance == nullptr) {
		return res;
	}

	btVector3 from(start.x, start.y, start.z);
	btVector3 to(start.x + dir.x, start.y + dir.y, start.z + dir.z);

	btCollisionWorld::ClosestRayResultCallback closestResults(from, to);
	closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

	m_instance->m_dynamicsWorld->rayTest(from, to, closestResults);
	if (closestResults.hasHit())
	{
		btVector3 hit_point = from.lerp(to, closestResults.m_closestHitFraction);

		res.did_hit = true;
		res.start = start;
		res.hit_point = to_glm_vector(hit_point);
		res.normal = to_glm_vector(closestResults.m_hitNormalWorld);
	}
	
	return res;
}

Physics::RayHitList Physics::RaycastAll(glm::vec3 start, glm::vec3 dir)
{
	RayHitList res;
	res.did_hit = false;

	if (m_instance == nullptr) {
		return res;
	}

	btVector3 from(start.x, start.y, start.z);
	btVector3 to(start.x + dir.x, start.y + dir.y, start.z + dir.z);

	btCollisionWorld::AllHitsRayResultCallback allResults(from, to);
	allResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
	allResults.m_flags |= btTriangleRaycastCallback::kF_KeepUnflippedNormal;
	//kF_UseGjkConvexRaytest flag is now enabled by default, use the faster but more approximate algorithm
	//allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;
	allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;

	m_instance->m_dynamicsWorld->rayTest(from, to, allResults);

	res.hits.reserve(allResults.m_hitFractions.size());

	for (int i = 0; i < allResults.m_hitFractions.size(); i++)
	{
		//tVector3 p = from.lerp(to, allResults.m_hitFractions[i]);

		btVector3 hit_point = from.lerp(to, allResults.m_hitFractions[i]);

		res.did_hit = true;

		Hit hit;
		hit.start = start;
		hit.hit_point = to_glm_vector(hit_point);
		hit.normal = to_glm_vector(allResults.m_hitNormalWorld[i]);
		res.hits.push_back(hit);
	}

	return res;
}
