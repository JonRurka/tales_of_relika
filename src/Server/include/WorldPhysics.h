#pragma once

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "Physics_base.h"

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

	static void StaticInit();
	static void StaticDispose();

	void Update(float dt);

	void Optimize()
	{
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
		
#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)
		optimize_jolt();
#endif
	}


#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	void Add_Shape(btCollisionShape* shape);

	void Add_Rigidbody(btRigidBody* body);

	void Remove_Rigidbody(btRigidBody* body);

	btBroadphaseInterface* Get_Broadphase() { return m_overlappingPairCache; }

	btDiscreteDynamicsWorld* GetDynamicWorld() { return m_dynamicsWorld; }
#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)

	void Add_Rigidbody(Body* body);

	void Remove_Rigidbody(Body* body);

	BodyInterface& GetBodyInterface() { return *mBodyInterface; }
	PhysicsSystem& GetPhysicsSystem() { return *mPhysicsSystem; }
	TempAllocator& GetTempAllocator() { return *mTempAllocator; }
	glm::vec3 glm_vec3(Vec3 val) {
		return glm::vec3(val.GetX(), val.GetY(), val.GetZ());
	}

#endif


	void Gravity(float val) { m_gravity = val; }
	float Gravity() { return m_gravity; }

	RayHit		Raycast(glm::vec3 from, glm::vec3 dir);
	RayHitList	RaycastAll(glm::vec3 from, glm::vec3 dir);

private:

	
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	btDefaultCollisionConfiguration* m_collisionConfiguration{ nullptr };
	btCollisionDispatcher* m_dispatcher{ nullptr };
	btBroadphaseInterface* m_overlappingPairCache{ nullptr };
	btSequentialImpulseConstraintSolver* m_solver{ nullptr };
	btDiscreteDynamicsWorld* m_dynamicsWorld{ nullptr };
	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

	RayHit		raycast_bullet(glm::vec3 from, glm::vec3 dir);
	RayHitList	raycastAll_bullet(glm::vec3 from, glm::vec3 dir);

#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)

	TempAllocator* mTempAllocator{ nullptr };
	JobSystemThreadPool* mJobSystem{ nullptr };
	JobSystemSingleThreaded* mJobSystemValidating{ nullptr };
	PhysicsSystem* mPhysicsSystem{ nullptr };
	BodyInterface* mBodyInterface{ nullptr };

	std::unordered_map<uint32_t, Body*> m_bodies;

	PhysicsSettings	mPhysicsSettings;



	RayHit		raycast_jolt(glm::vec3 from, glm::vec3 dir);
	RayHitList	raycastAll_jolt(glm::vec3 from, glm::vec3 dir);

	void optimize_jolt();


#endif

	static bool m_static_inited;

	double m_last_update{ 0 };
	float m_gravity{ DEFAULT_GRAVITY };
	bool m_initialied{ false };
	

	inline static const std::string LOG_LOC{ "SERVER_WORLD_PHYSICS" };
};