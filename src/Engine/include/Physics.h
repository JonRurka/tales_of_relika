#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

class Engine;
class Collider;
class BoxCollider;

class Physics {
	friend class Engine;
	friend class Collider;
	friend class BoxCollider;
public:

	struct RayHit {
		bool did_hit;
		glm::vec3 start;
		glm::vec3 hit_point;
		glm::vec3 normal;
	};

	struct Hit {
		glm::vec3 start;
		glm::vec3 hit_point;
		glm::vec3 normal;
	};

	struct RayHitList {
		bool did_hit;
		std::vector<Hit> hits;
	};

	static void Init();

	static RayHit		Raycast(glm::vec3 from, glm::vec3 dir);
	static RayHitList	RaycastAll(glm::vec3 from, glm::vec3 dir);

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

	static void Remove_Rigidbody(btRigidBody* body) { m_instance->remove_rigidbody(body); }
	void remove_rigidbody(btRigidBody* body);
};