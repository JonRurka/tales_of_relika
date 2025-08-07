#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

#define DEFAULT_GRAVITY (-10.0f)

class Engine;
class Collider;
class BoxCollider;

class Physics {
	friend class Engine;
	friend class Collider;
	friend class BoxCollider;
public:

	struct RayHit {
		bool did_hit{ false };
		Collider* collider{ nullptr };
		glm::vec3 start{glm::vec3()};
		glm::vec3 hit_point{ glm::vec3() };
		glm::vec3 normal{ glm::vec3() };
	};

	struct Hit {
		glm::vec3 start{ glm::vec3() };
		glm::vec3 hit_point{ glm::vec3() };
		glm::vec3 normal{ glm::vec3() };
	};

	struct RayHitList {
		bool did_hit{ false };
		std::vector<Hit> hits;
	};

	static Physics& Instance()
	{
		static Physics inst;
		return inst;
	}

	void Init();

	static RayHit		Raycast(glm::vec3 from, glm::vec3 dir);
	static RayHitList	RaycastAll(glm::vec3 from, glm::vec3 dir);

	static btBroadphaseInterface& Get_Broadphase() { return *Instance().m_overlappingPairCache; }

	static btDiscreteDynamicsWorld& GetDynamicWorld() { return *Instance().m_dynamicsWorld; }

	static void Gravity(float val) { Instance().m_gravity = val; }
	static float Gravity() { return Instance().m_gravity; }
	
private:
	Physics() = default;

	std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfiguration{ nullptr };
	std::unique_ptr<btCollisionDispatcher> m_dispatcher{ nullptr };
	std::unique_ptr<btBroadphaseInterface> m_overlappingPairCache{ nullptr };
	std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver{ nullptr };
	std::unique_ptr<btDiscreteDynamicsWorld> m_dynamicsWorld{ nullptr };

	double m_last_update{ 0 };
	float m_gravity{ DEFAULT_GRAVITY };
	bool m_initialied{ false };

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> m_collisionShapes_box;

	void update_internal(float dt);

	static void Add_Box_Shape(btCollisionShape* shape) { return Instance().add_box_shape(shape); }
	void add_box_shape(btCollisionShape* shape);

	static void Add_Rigidbody(btRigidBody* body) { Instance().add_rigidbody(body); }
	void add_rigidbody(btRigidBody* body);

	//static void Add_Object();

	static void Remove_Rigidbody(btRigidBody* body) { Instance().remove_rigidbody(body); }
	void remove_rigidbody(btRigidBody* body);

	inline static const std::string LOG_LOC{ "PHYSICS" };
};