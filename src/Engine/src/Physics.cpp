#include "Physics.h"

#include "Logger.h"
#include "Utilities.h"

#define UPDATE_INTERVAL (1.f / 60.f)


#define JOLT_TEMP_ALLOCATOR_SIZE (32 * 1024 * 1024)
#define JOLT_SIMULATION_STEPS 1

// All Jolt symbols are in the JPH namespace
using namespace JPH;

bool Physics::m_static_inited{ false };

namespace {
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	btVector3 to_bt_vector(glm::vec3 value) {
		return btVector3(value.x, value.y, value.z);
	}

	glm::vec3 to_glm_vector(btVector3 value) {
		return glm::vec3(value.x(), value.y(), value.z());
	}

	void kinematicPreTickCallback(btDynamicsWorld* world, btScalar deltaTime)
	{
		btRigidBody* groundBody = (btRigidBody*)world->getWorldUserInfo();
		btTransform predictedTrans;
		btVector3 linearVelocity(0, 0, 0);
		btVector3 angularVelocity(0, 0.1, 0);
		btTransformUtil::integrateTransform(groundBody->getWorldTransform(), linearVelocity, angularVelocity, deltaTime, predictedTrans);
		groundBody->getMotionState()->setWorldTransform(predictedTrans);

	}
#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)
	static const uint cNumBodies = 10240;
	static const uint cNumBodyMutexes = 0; // Autodetect
	static const uint cMaxBodyPairs = 65536;
	static const uint cMaxContactConstraints = 20480;

	/// Broadphase layers
	namespace BroadPhaseLayers
	{
		static constexpr BroadPhaseLayer NON_MOVING(0);
		static constexpr BroadPhaseLayer MOVING(1);
		static constexpr BroadPhaseLayer DEBRIS(2);
		static constexpr BroadPhaseLayer SENSOR(3);
		static constexpr BroadPhaseLayer UNUSED(4);
		static constexpr uint NUM_LAYERS(5);
	};

	/// BroadPhaseLayerInterface implementation
	class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
	{
	public:
		BPLayerInterfaceImpl()
		{
			// Create a mapping table from object to broad phase layer
			mObjectToBroadPhase[Layers::UNUSED1] = BroadPhaseLayers::UNUSED;
			mObjectToBroadPhase[Layers::UNUSED2] = BroadPhaseLayers::UNUSED;
			mObjectToBroadPhase[Layers::UNUSED3] = BroadPhaseLayers::UNUSED;
			mObjectToBroadPhase[Layers::UNUSED4] = BroadPhaseLayers::UNUSED;
			mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
			mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
			mObjectToBroadPhase[Layers::DEBRIS] = BroadPhaseLayers::DEBRIS;
			mObjectToBroadPhase[Layers::SENSOR] = BroadPhaseLayers::SENSOR;
		}

		virtual uint					GetNumBroadPhaseLayers() const override
		{
			return BroadPhaseLayers::NUM_LAYERS;
		}

		virtual BroadPhaseLayer			GetBroadPhaseLayer(ObjectLayer inLayer) const override
		{
			JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
			return mObjectToBroadPhase[inLayer];
		}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
		{
			switch ((BroadPhaseLayer::Type)inLayer)
			{
			case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
			case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
			case (BroadPhaseLayer::Type)BroadPhaseLayers::DEBRIS:		return "DEBRIS";
			case (BroadPhaseLayer::Type)BroadPhaseLayers::SENSOR:		return "SENSOR";
			case (BroadPhaseLayer::Type)BroadPhaseLayers::UNUSED:		return "UNUSED";
			default:													JPH_ASSERT(false); return "INVALID";
			}
		}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

	private:
		BroadPhaseLayer					mObjectToBroadPhase[Layers::NUM_LAYERS];
	} gBroadPhaseLayerInterface;
	

	/// Class that determines if two object layers can collide
	class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
	{
	public:
		virtual bool					ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
		{
			switch (inObject1)
			{
			case Layers::UNUSED1:
			case Layers::UNUSED2:
			case Layers::UNUSED3:
			case Layers::UNUSED4:
				return false;
			case Layers::NON_MOVING:
				return inObject2 == Layers::MOVING || inObject2 == Layers::DEBRIS;
			case Layers::MOVING:
				return inObject2 == Layers::NON_MOVING || inObject2 == Layers::MOVING || inObject2 == Layers::SENSOR;
			case Layers::DEBRIS:
				return inObject2 == Layers::NON_MOVING;
			case Layers::SENSOR:
				return inObject2 == Layers::MOVING;
			default:
				JPH_ASSERT(false);
				return false;
			}
		}
	} gObjectVsObjectLayerFilter;

	/// Class that determines if an object layer can collide with a broadphase layer
	class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
	{
	public:
		virtual bool					ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
		{
			switch (inLayer1)
			{
			case Layers::NON_MOVING:
				return inLayer2 == BroadPhaseLayers::MOVING || inLayer2 == BroadPhaseLayers::DEBRIS;
			case Layers::MOVING:
				return inLayer2 == BroadPhaseLayers::NON_MOVING || inLayer2 == BroadPhaseLayers::MOVING || inLayer2 == BroadPhaseLayers::SENSOR;
			case Layers::DEBRIS:
				return inLayer2 == BroadPhaseLayers::NON_MOVING;
			case Layers::SENSOR:
				return inLayer2 == BroadPhaseLayers::MOVING;
			case Layers::UNUSED1:
			case Layers::UNUSED2:
			case Layers::UNUSED3:
				return false;
			default:
				JPH_ASSERT(false);
				return false;
			}
		}
	} gObjectVsBroadPhaseLayerFilter;


	//gObjectVsObjectLayerFilter

#endif
}



void Physics::update_internal(float dt)
{
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	float time = Utilities::Get_Time() - m_last_update;
	if (time < UPDATE_INTERVAL) {
		return;
	}
	//Logger::LogDebug(LOG_POS("update_internal"), "physics update: %f", time * 1000);
	m_last_update = Utilities::Get_Time();

	m_dynamicsWorld->updateAabbs();
	m_dynamicsWorld->computeOverlappingPairs();
	m_dynamicsWorld->stepSimulation(time, 10, UPDATE_INTERVAL);

#elif (PHYSICS_BACKEND == PHYSICS_BACKEND_JOLT)

	float time = Utilities::Get_Time() - m_last_update;
	if (time < UPDATE_INTERVAL) {
		return;
	}
	//Logger::LogDebug(LOG_POS("update_internal"), "physics update: %f", time * 1000);
	m_last_update = Utilities::Get_Time();


	mPhysicsSystem->Update(time, JOLT_SIMULATION_STEPS, mTempAllocator.get(), mJobSystem.get());


#endif

	//Logger::LogDebug(LOG_POS("update_internal"), "Num objects: %i", m_dynamicsWorld->getNumCollisionObjects());


	
}


void Physics::StaticInit()
{
	if (m_static_inited)
		return;
	
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)

#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)
	RegisterDefaultAllocator();
	Factory::sInstance = new Factory;
	RegisterTypes();
#endif
	
	m_static_inited = true;
}

void Physics::StaticDispose()
{
	if (!m_static_inited)
		return;

	UnregisterTypes();

	delete Factory::sInstance;
	Factory::sInstance = nullptr;

	m_static_inited = false;
}

void Physics::Init()
{
	StaticInit();


#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	m_collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	m_dispatcher = std::make_unique<btCollisionDispatcher>(m_collisionConfiguration.get());

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	m_overlappingPairCache = std::make_unique<btDbvtBroadphase>();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	m_solver = std::make_unique<btSequentialImpulseConstraintSolver>();

	m_dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(m_dispatcher.get(), m_overlappingPairCache.get(), m_solver.get(), m_collisionConfiguration.get());
	m_dynamicsWorld->setGravity(btVector3(0, -10, 0));

#elif(PHYSICS_BACKEND == PHYSICS_BACKEND_JOLT)

	mTempAllocator = std::make_unique<TempAllocatorImpl>(JOLT_TEMP_ALLOCATOR_SIZE);
	
	mJobSystem = std::make_unique<JobSystemThreadPool>(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);
	mJobSystemValidating = std::make_unique<JobSystemSingleThreaded>(cMaxPhysicsJobs);

	mPhysicsSystem = std::make_unique<PhysicsSystem>();
	mPhysicsSystem->Init(cNumBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, gBroadPhaseLayerInterface, gObjectVsBroadPhaseLayerFilter, gObjectVsObjectLayerFilter);
	mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);
	mPhysicsSystem->SetGravity(Vec3(0, -9.81f, 0));
	mBodyInterface = std::unique_ptr<BodyInterface>(&mPhysicsSystem->GetBodyInterface());

#endif

	m_last_update = Utilities::Get_Time();

	Logger::LogInfo(LOG_POS("INIT"), "Physics Initialized.");

	m_initialied = true;
}

float Physics::Fixed_DeltaTime()
{
	return UPDATE_INTERVAL;
}



#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)


Physics::RayHit Physics::raycast_bullet(glm::vec3 start, glm::vec3 dir)
{
	RayHit res{};
	res.did_hit = false;
	res.start = start;

	if (!Instance().m_initialied) {
		return res;
	}

	btVector3 from(start.x, start.y, start.z);
	btVector3 to(start.x + dir.x, start.y + dir.y, start.z + dir.z);

	btCollisionWorld::ClosestRayResultCallback closestResults(from, to);
	//closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

	Instance().m_dynamicsWorld->rayTest(from, to, closestResults);
	if (closestResults.hasHit())
	{
		btVector3 hit_point = from.lerp(to, closestResults.m_closestHitFraction);

		res.did_hit = true;
		res.collider = (Collider*)closestResults.m_collisionObject->getUserPointer();
		res.start = start;
		res.hit_point = to_glm_vector(hit_point);
		res.normal = to_glm_vector(closestResults.m_hitNormalWorld);

		//Logger::LogDebug(LOG_POS("Raycast"), "Did Hit");
	}
	else {
		//Logger::LogDebug(LOG_POS("Raycast"), "Not Hit");
	}

	return res;
}

Physics::RayHitList Physics::raycastAll_bullet(glm::vec3 start, glm::vec3 dir)
{
	RayHitList res{};
	res.did_hit = false;

	if (!Instance().m_initialied) {
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

	Instance().m_dynamicsWorld->rayTest(from, to, allResults);

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
#elif (PHYSICS_BACKEND == PHYSICS_BACKEND_JOLT)

Physics::RayHit Physics::raycast_jolt(glm::vec3 from, glm::vec3 dir)
{
	const BroadPhaseQuery& broadphase = Instance().mPhysicsSystem->GetBroadPhaseQuery();
	


	return RayHit();
}

Physics::RayHitList Physics::raycastAll_jolt(glm::vec3 from, glm::vec3 dir)
{
	return RayHitList();
}

void Physics::optimize_jolt()
{
	Instance().mPhysicsSystem->OptimizeBroadPhase();
}


#endif
