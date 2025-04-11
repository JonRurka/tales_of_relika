#pragma once

#include "btBulletDynamicsCommon.h"

class Engine;
class Collider;
class BoxCollider;

class Physics {
	friend class Engine;
	friend class Collider;
	friend class BoxCollider;
public:
	static void Init();

private:
	Physics();

	btDefaultCollisionConfiguration* m_collisionConfiguration{ nullptr };
	btCollisionDispatcher* m_dispatcher{ nullptr };
	btBroadphaseInterface* m_overlappingPairCache{ nullptr };
	btSequentialImpulseConstraintSolver* m_solver{ nullptr };
	btDiscreteDynamicsWorld* m_dynamicsWorld{ nullptr };

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> m_collisionShapes_box;

	static Physics* m_instance;

	void update_internal(float dt);

	static void Add_Box_Shape(btCollisionShape* shape) { return m_instance->add_box_shape(shape); }
	void add_box_shape(btCollisionShape* shape);

	static void Add_Rigidbody(btRigidBody* body) { m_instance->add_rigidbody(body); }
	void add_rigidbody(btRigidBody* body);
};