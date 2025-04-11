#include "Physics.h"

Physics* Physics::m_instance{nullptr};

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

	
}

void Physics::update_internal(float dt)
{
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


void Physics::Init()
{
	m_instance = new Physics();
}
