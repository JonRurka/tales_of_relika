#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/PhysicsMaterialSimple.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

// All Jolt symbols are in the JPH namespace
using namespace JPH;

#endif

#define DEFAULT_GRAVITY (-10.0f)

class Engine;
class Collider;
class BoxCollider;

namespace Layers
{
	static constexpr ObjectLayer UNUSED1 = 0; // 4 unused values so that broadphase layers values don't match with object layer values (for testing purposes)
	static constexpr ObjectLayer UNUSED2 = 1;
	static constexpr ObjectLayer UNUSED3 = 2;
	static constexpr ObjectLayer UNUSED4 = 3;
	static constexpr ObjectLayer NON_MOVING = 4;
	static constexpr ObjectLayer MOVING = 5;
	static constexpr ObjectLayer DEBRIS = 6; // Example: Debris collides only with NON_MOVING
	static constexpr ObjectLayer SENSOR = 7; // Sensors only collide with MOVING objects
	static constexpr ObjectLayer NUM_LAYERS = 8;
};

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
	void Init();

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
#else
		optimize_jolt();
#endif
	}




#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	static btBroadphaseInterface& Get_Broadphase() { return *Instance().m_overlappingPairCache; }
	static btDiscreteDynamicsWorld& GetDynamicWorld() { return *Instance().m_dynamicsWorld; }
#else
	static BodyInterface& GetBodyInterface() { return *Instance().mBodyInterface; }
	static PhysicsSystem& GetPhysicsSystem() { return *Instance().mPhysicsSystem; }
	static TempAllocator& GetTempAllocator() { return *Instance().mTempAllocator; }
	static glm::vec3 glm_vec3(Vec3 val) {
		return glm::vec3(val.GetX(), val.GetY(), val.GetZ());
	}
#endif




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

#else

	std::unique_ptr<TempAllocator> mTempAllocator;
	std::unique_ptr<JobSystemThreadPool> mJobSystem;
	std::unique_ptr<JobSystemSingleThreaded> mJobSystemValidating;
	std::unique_ptr<PhysicsSystem> mPhysicsSystem;
	std::unique_ptr<BodyInterface> mBodyInterface;

	PhysicsSettings	mPhysicsSettings;



	static RayHit		raycast_jolt(glm::vec3 from, glm::vec3 dir);
	static RayHitList	raycastAll_jolt(glm::vec3 from, glm::vec3 dir);

	static void optimize_jolt();

#endif

	static bool m_static_inited;

	double m_last_update{ 0 };
	float m_gravity{ DEFAULT_GRAVITY };
	bool m_initialied{ false };

	

	void update_internal(float dt);



	inline static const std::string LOG_LOC{ "PHYSICS" };
};