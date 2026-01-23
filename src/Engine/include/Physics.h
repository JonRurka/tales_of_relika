#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <queue>

#include "Physics_base.h"

class Engine;
class Collider;
class BoxCollider;
class WorldObject;

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

	static void StaticInit();
	static void StaticDispose();
	void Init(bool async);

	static float Fixed_DeltaTime();


	static RayHit		Raycast(glm::vec3 from, glm::vec3 dir) {
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
		return raycast_bullet(from, dir);
#else
		return raycast_jolt(from, dir);
#endif
	}
	static RayHitList	RaycastAll(glm::vec3 from, glm::vec3 dir) {
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
		return raycast_bullet(from, dir);
#else
		return raycastAll_jolt(from, dir);
#endif
	}


	void Optimize()
	{
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
		optimize_bullet();
#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)
		optimize_jolt();
#endif
	}




#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	static btBroadphaseInterface& Get_Broadphase() { return *Instance().m_overlappingPairCache; }
	static btDiscreteDynamicsWorld& GetDynamicWorld() { return *Instance().m_dynamicsWorld; }
#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)

	// Don't call from fixed update. Thread-safe add call.
	void Request_Rigidbody(std::weak_ptr<Collider> col, BodyCreationSettings shape_settings); // add shape to queue to be processed

	void Remove_Rigidbody(Body* body);

	// DO ONLY call from fixed update, not thread-safe.
	uint32_t Add_Collider(std::weak_ptr<Collider> col, bool do_lock = true);

	void Remove_Collider(uint32_t id, bool do_lock = true);
		
	static BodyInterface& GetBodyInterface() { return *Instance().mBodyInterface; }
	static PhysicsSystem& GetPhysicsSystem() { return *Instance().mPhysicsSystem; }
	static TempAllocator& GetTempAllocator() { return *Instance().mTempAllocator; }
	static glm::vec3 glm_vec3(Vec3 val) {
		return glm::vec3(val.GetX(), val.GetY(), val.GetZ());
	}
#endif


	~Physics();

	static void Gravity(float val) { Instance().m_gravity = val; }
	static float Gravity() { return Instance().m_gravity; }
	
private:
	Physics() = default;

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfiguration{ nullptr };
	std::unique_ptr<btCollisionDispatcher> m_dispatcher{ nullptr };
	std::unique_ptr<btBroadphaseInterface> m_overlappingPairCache{ nullptr };
	std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver{ nullptr };
	std::unique_ptr<btDiscreteDynamicsWorld> m_dynamicsWorld{ nullptr };

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> m_collisionShapes_box;

	static void Add_Box_Shape(btCollisionShape* shape) { return Instance().add_box_shape(shape); }
	void add_box_shape(btCollisionShape* shape);

	static void Add_Rigidbody(btRigidBody* body) { Instance().add_rigidbody(body); }
	void add_rigidbody(btRigidBody* body);

	//static void Add_Object();

	static void Remove_Rigidbody(btRigidBody* body) { Instance().remove_rigidbody(body); }
	void remove_rigidbody(btRigidBody* body);

	static RayHit		raycast_bullet(glm::vec3 from, glm::vec3 dir);
	static RayHitList	raycastAll_bullet(glm::vec3 from, glm::vec3 dir);

	static void optimize_bullet(){}

#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)

	TempAllocator* mTempAllocator{nullptr};
	JobSystemThreadPool* mJobSystem{ nullptr };
	JobSystemSingleThreaded* mJobSystemValidating{ nullptr };
	PhysicsSystem* mPhysicsSystem{ nullptr };
	BodyInterface* mBodyInterface{ nullptr };

	struct body_ref{
		Body* RBody;
		std::string Object_Name;
		std::weak_ptr<Collider> Col;
		std::shared_ptr<WorldObject> Obj;
	};
	struct collider_ref {
		std::string Object_Name;
		std::weak_ptr<Collider> Col;
		std::shared_ptr<WorldObject> Obj;
	};
	std::unordered_map<uint32_t, body_ref> m_bodies;

	uint32_t m_char_latest_id = 0;
	std::unordered_map<uint32_t, collider_ref> m_colliders;

	PhysicsSettings	mPhysicsSettings;

	std::vector<BodyID> m_bodies_to_add;

	struct rigidbody_request {
		BodyCreationSettings ShapeSettings;
		std::weak_ptr<Collider> Col;
	};
	std::queue<rigidbody_request> m_rigidbody_req_queue;


	std::queue<Body*> m_rigidbody_rem_queue;

	static RayHit		raycast_jolt(glm::vec3 from, glm::vec3 dir);
	static RayHitList	raycastAll_jolt(glm::vec3 from, glm::vec3 dir);

	static void optimize_jolt();

	void add_rigidbody_internal(std::weak_ptr<Collider> col, Body* body);

	// Do not call from Fixed Update.
	bool remove_rigidbody_internal(Body* body);

#endif

	bool m_is_async{ false };
	std::mutex m_lock;

	static bool m_static_inited;

	double m_last_update{ 0 };
	float m_gravity{ DEFAULT_GRAVITY };
	bool m_initialied{ false };
	volatile bool m_running{ false };
	std::thread m_thread;

	double m_debug_print_timer{ .5 };
	
	static void RunAsync(Physics* phy);
	void update_internal(float dt);



	inline static const std::string LOG_LOC{ "PHYSICS" };
};