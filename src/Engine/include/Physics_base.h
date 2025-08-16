#pragma once

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
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

// All Jolt symbols are in the JPH namespace
using namespace JPH;

#endif

#define DEFAULT_GRAVITY (-10.0f)

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