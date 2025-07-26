#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

#define DEFAULT_GRAVITY (-10.0f)

class WorldPhysics {
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

	WorldPhysics();
	void Init();

	void Update(float dt);

	void Add_Shape(btCollisionShape* shape);

	void Add_Rigidbody(btRigidBody* body);

	void Remove_Rigidbody(btRigidBody* body);

	btBroadphaseInterface* Get_Broadphase() { return m_overlappingPairCache; }

	btDiscreteDynamicsWorld* GetDynamicWorld() { return m_dynamicsWorld; }

	void Gravity(float val) { m_gravity = val; }
	float Gravity() { return m_gravity; }

	RayHit		Raycast(glm::vec3 from, glm::vec3 dir);
	RayHitList	RaycastAll(glm::vec3 from, glm::vec3 dir);

private:

	

	btDefaultCollisionConfiguration* m_collisionConfiguration{ nullptr };
	btCollisionDispatcher* m_dispatcher{ nullptr };
	btBroadphaseInterface* m_overlappingPairCache{ nullptr };
	btSequentialImpulseConstraintSolver* m_solver{ nullptr };
	btDiscreteDynamicsWorld* m_dynamicsWorld{ nullptr };

	double m_last_update{ 0 };
	float m_gravity{ DEFAULT_GRAVITY };

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

	inline static const std::string LOG_LOC{ "SERVER_WORLD_PHYSICS" };
};