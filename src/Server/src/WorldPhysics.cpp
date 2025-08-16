#include "WorldPhysics.h"

#include "Utilities.h"
#include "Logger.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"

#define UPDATE_INTERVAL (1.f / 60.f)
#define JOLT_TEMP_ALLOCATOR_SIZE (32 * 1024 * 1024)
#define JOLT_SIMULATION_STEPS 1

bool WorldPhysics::m_static_inited{ false };

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
	Vec3 to_jolt_vector(glm::vec3 value) {
		return Vec3(value.x, value.y, value.z);
	}

	glm::vec3 to_glm_vector(Vec3 value) {
		return glm::vec3(value.GetX(), value.GetY(), value.GetZ());
	}
	
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


#endif
}

WorldPhysics::WorldPhysics()
{
}

void WorldPhysics::StaticInit()
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

void WorldPhysics::StaticDispose()
{
	if (!m_static_inited)
		return;

	UnregisterTypes();

	delete Factory::sInstance;
	Factory::sInstance = nullptr;

	m_static_inited = false;
}

void WorldPhysics::Init()
{
	StaticInit();

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
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

	Get_Broadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)

	mTempAllocator = new TempAllocatorImpl(JOLT_TEMP_ALLOCATOR_SIZE);

	mJobSystem = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);
	mJobSystemValidating = new JobSystemSingleThreaded(cMaxPhysicsJobs);

	mPhysicsSystem = new PhysicsSystem();
	mPhysicsSystem->Init(cNumBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, gBroadPhaseLayerInterface, gObjectVsBroadPhaseLayerFilter, gObjectVsObjectLayerFilter);
	mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);
	mPhysicsSystem->SetGravity(Vec3(0, -9.81f, 0));
	mBodyInterface = &mPhysicsSystem->GetBodyInterface();

#endif

	m_last_update = Utilities::Get_Time();

	Logger::LogInfo(LOG_POS("INIT"), "Physics Initialized.");

	m_initialied = true;
}

void WorldPhysics::Update(float dt)
{
	float time = Utilities::Get_Time() - m_last_update;
	if (time < UPDATE_INTERVAL) {
		return;
	}
	//Logger::LogDebug(LOG_POS("update_internal"), "physics update: %f", time * 1000);
	m_last_update = Utilities::Get_Time();

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	m_dynamicsWorld->updateAabbs();
	m_dynamicsWorld->computeOverlappingPairs();
	m_dynamicsWorld->stepSimulation(time, 10, UPDATE_INTERVAL);

#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)
	mPhysicsSystem->Update(time, JOLT_SIMULATION_STEPS, mTempAllocator, mJobSystem);

#endif
}

WorldPhysics::RayHit WorldPhysics::Raycast(glm::vec3 from, glm::vec3 dir)
{
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	return raycast_bullet(from, dir);
#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)
	return raycast_jolt(from, dir);
#endif
}

WorldPhysics::RayHitList WorldPhysics::RaycastAll(glm::vec3 from, glm::vec3 dir)
{
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	return raycastAll_bullet(from, dir);
#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)
	return raycastAll_jolt(from, dir);
#endif
}


#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
void WorldPhysics::Add_Shape(btCollisionShape* shape)
{
	m_collisionShapes.push_back(shape);
}

void WorldPhysics::Add_Rigidbody(btRigidBody* body)
{
	m_dynamicsWorld->addRigidBody(body);
}

void WorldPhysics::Remove_Rigidbody(btRigidBody* body)
{
	btCollisionObject* obj = (btCollisionObject*)body;
	if (body && body->getMotionState())
	{
		delete body->getMotionState();
	}
	m_dynamicsWorld->removeCollisionObject(body);
	delete body;
}

WorldPhysics::RayHit WorldPhysics::raycast_bullet(glm::vec3 start, glm::vec3 dir)
{
	RayHit res;
	res.did_hit = false;
	res.start = start;

	btVector3 from(start.x, start.y, start.z);
	btVector3 to(start.x + dir.x, start.y + dir.y, start.z + dir.z);

	btCollisionWorld::ClosestRayResultCallback closestResults(from, to);
	//closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

	m_dynamicsWorld->rayTest(from, to, closestResults);
	if (closestResults.hasHit())
	{
		btVector3 hit_point = from.lerp(to, closestResults.m_closestHitFraction);

		res.did_hit = true;
		//res.collider = (Collider*)closestResults.m_collisionObject->getUserPointer();
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

WorldPhysics::RayHitList WorldPhysics::raycastAll_bullet(glm::vec3 start, glm::vec3 dir)
{
	RayHitList res;
	res.did_hit = false;

	btVector3 from(start.x, start.y, start.z);
	btVector3 to(start.x + dir.x, start.y + dir.y, start.z + dir.z);

	btCollisionWorld::AllHitsRayResultCallback allResults(from, to);
	allResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
	allResults.m_flags |= btTriangleRaycastCallback::kF_KeepUnflippedNormal;
	//kF_UseGjkConvexRaytest flag is now enabled by default, use the faster but more approximate algorithm
	//allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;
	allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;

	m_dynamicsWorld->rayTest(from, to, allResults);

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

#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)

void WorldPhysics::Add_Rigidbody(Body* body)
{
	m_bodies[body->GetID().GetIndex()] = body;
}

void WorldPhysics::Remove_Rigidbody(Body* body)
{
	if (m_bodies.contains(body->GetID().GetIndex()))
	{
		m_bodies.erase(body->GetID().GetIndex());
	}
}

WorldPhysics::RayHit WorldPhysics::raycast_jolt(glm::vec3 from, glm::vec3 dir)
{
	RayHit res{};
	res.did_hit = false;
	res.start = from;

	if (!m_initialied) {
		return res;
	}

	RRayCast ray = RRayCast( Vec3(from.x, from.y, from.z), Vec3(dir.x, dir.y, dir.z) );
	const BroadPhaseQuery& broadphase = mPhysicsSystem->GetBroadPhaseQuery();
	const NarrowPhaseQuery& narrowphase = mPhysicsSystem->GetNarrowPhaseQuery();
	RayCastResult hit_result;
	bool did_hit = narrowphase.CastRay(ray, hit_result);

	if (!did_hit)
	{
		return res;
	}
	
	assert(m_bodies.contains(hit_result.mBodyID.GetIndex()));
	const JPH::Body* body = m_bodies[hit_result.mBodyID.GetIndex()];
	Vec3 hit_point = ray.GetPointOnRay(hit_result.mFraction);

	res.did_hit = true;
	res.start = from;
	res.hit_point = to_glm_vector(hit_point);
	res.normal = to_glm_vector(body->GetWorldSpaceSurfaceNormal(hit_result.mSubShapeID2, hit_point));
	
	return res;
}


WorldPhysics::RayHitList WorldPhysics::raycastAll_jolt(glm::vec3 from, glm::vec3 dir)
{
	RayHitList res{};
	res.did_hit = false;

	if (!m_initialied) {
		return res;
	}

	return res;
}

void WorldPhysics::optimize_jolt()
{
	mPhysicsSystem->OptimizeBroadPhase();
}





#endif

