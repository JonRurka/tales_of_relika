#include "CharacterCollider.h"

#include "WorldObject.h"
#include "Transform.h"
#include "Logger.h"
#include "Graphics.h"

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
#include"BulletCollision/CollisionDispatch/btGhostObject.h"
#include"BulletDynamics/Character/btKinematicCharacterController.h"
#endif

void CharacterCollider::Init()
{
	base_Init();

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	m_shape = std::make_unique<btCapsuleShapeZ>(m_radius, m_height);
#else
	mStandingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightStanding + cCharacterRadiusStanding, 0), Quat::sIdentity(), new CapsuleShape(0.5f * cCharacterHeightStanding, cCharacterRadiusStanding)).Create().Get();
	mCrouchingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightCrouching + cCharacterRadiusCrouching, 0), Quat::sIdentity(), new CapsuleShape(0.5f * cCharacterHeightCrouching, cCharacterRadiusCrouching)).Create().Get();
	mInnerStandingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightStanding + cCharacterRadiusStanding, 0), Quat::sIdentity(), new CapsuleShape(0.5f * cInnerShapeFraction * cCharacterHeightStanding, cInnerShapeFraction * cCharacterRadiusStanding)).Create().Get();
	mInnerCrouchingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightCrouching + cCharacterRadiusCrouching, 0), Quat::sIdentity(), new CapsuleShape(0.5f * cInnerShapeFraction * cCharacterHeightCrouching, cInnerShapeFraction * cCharacterRadiusCrouching)).Create().Get();
#endif
}

void CharacterCollider::Update(float dt)
{
	m_lock.lock();
	glm::fvec3 pos = glm::fvec3(m_pos.GetX(), m_pos.GetY(), m_pos.GetZ());
	m_lock.unlock();

	Object().Get_Transform().Position(pos);
}

void CharacterCollider::Radius(float radius) 
{
	cCharacterRadiusStanding = radius;
	cCharacterRadiusCrouching = radius;

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	m_radius = radius;
	m_shape.reset();
	m_shape = std::make_unique<btCapsuleShapeZ>(m_radius, m_height);
#else
	mStandingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightStanding + cCharacterRadiusStanding, 0), Quat::sIdentity(), new CapsuleShape(0.5f * cCharacterHeightStanding, cCharacterRadiusStanding)).Create().Get();
	mCrouchingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightCrouching + cCharacterRadiusCrouching, 0), Quat::sIdentity(), new CapsuleShape(0.5f * cCharacterHeightCrouching, cCharacterRadiusCrouching)).Create().Get();
	mInnerStandingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightStanding + cCharacterRadiusStanding, 0), Quat::sIdentity(), new CapsuleShape(0.5f * cInnerShapeFraction * cCharacterHeightStanding, cInnerShapeFraction * cCharacterRadiusStanding)).Create().Get();
	mInnerCrouchingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightCrouching + cCharacterRadiusCrouching, 0), Quat::sIdentity(), new CapsuleShape(0.5f * cInnerShapeFraction * cCharacterHeightCrouching, cInnerShapeFraction * cCharacterRadiusCrouching)).Create().Get();
#endif
}

void CharacterCollider::Height(float height) 
{
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	m_height = height;
	m_shape.reset();
	m_shape = std::make_unique<btCapsuleShapeZ>(m_radius, m_height);
#else





#endif
}

void CharacterCollider::Set_Location(glm::vec3 pos)
{
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	btVector3 bt_new_pos = btVector3(pos.x, pos.y, pos.z);
	Get_Controller().warp(bt_new_pos);
#else
	Get_Controller().SetPosition(Vec3(pos.x, pos.y, pos.z));
#endif
}

void CharacterCollider::FixedUpdate(float dt)
{
	//Logger::LogDebug(LOG_POS("FixedUpdate"), "call fixed update.");

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	if (m_charCon == nullptr)
		return;

	btTransform t;
	t = m_charCon->getGhostObject()->getWorldTransform();
	btVector3 pos = t.getOrigin();
	btQuaternion  quat = t.getRotation();

	Object().Get_Transform().Position(glm::fvec3(pos.x(), pos.y(), pos.z()));
#else

	if (m_reset_character) {
		m_reset_character = false;
		mCharacter = new CharacterVirtual(mSettings, m_reset_pos, Quat::sIdentity(), 0, &Physics::GetPhysicsSystem());
		mCharacter->SetCharacterVsCharacterCollision(&mCharacterVsCharacterCollision);
		m_has_character = true;
		Logger::LogDebug(LOG_POS("FixedUpdate"), "Refreshed character controller.");
	}

	if (mCharacter.GetPtr() == nullptr)
		return;

	CharacterVirtual::ExtendedUpdateSettings update_settings;
	// Update the character position
	mCharacter->ExtendedUpdate(Physics::Fixed_DeltaTime(),
		-mCharacter->GetUp() * Physics::GetPhysicsSystem().GetGravity().Length(),
		update_settings,
		Physics::GetPhysicsSystem().GetDefaultBroadPhaseLayerFilter(Layers::MOVING),
		Physics::GetPhysicsSystem().GetDefaultLayerFilter(Layers::MOVING),
		{ },
		{ },
		Physics::GetTempAllocator());

	m_lock.lock();
	m_pos = mCharacter->GetPosition();
	m_rot = mCharacter->GetRotation();
	m_lock.unlock();
	


#endif
	//Object()->Get_Transform()->Rotation(glm::quat(quat.x(), quat.y(), quat.z(), quat.w()));

	//Logger::LogDebug(LOG_POS("Update"), "(%f, %f, %f)",
	//	pos.x(), pos.y(), pos.z());

	//btVector3 min;
	//btVector3 max;
	//m_charCon->getGhostObject()->getCollisionShape()->getAabb(t, min, max);
	//Graphics::DrawDebugRay(glm::vec3(min.x(), min.y(), min.z()), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
	//Graphics::DrawDebugRay(glm::vec3(max.x(), max.y(), max.z()), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
}

void CharacterCollider::Load(json data)
{
}

void CharacterCollider::OnDestroy()
{
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	Physics::GetDynamicWorld().removeAction(m_charCon.get());
	Physics::GetDynamicWorld().removeCollisionObject(m_ghostObject.get());

	m_charCon.reset();
	m_ghostObject.reset();
	m_shape.reset();
#else
	
	m_has_character = false;




#endif
}

void CharacterCollider::OnRefresh()
{
	if (!Active())
		return;


#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	if (Is_Dynamic()) {
		m_shape->calculateLocalInertia(Mass(), m_localInertia);
	}
	else {
		m_localInertia = btVector3(0.0f, 0.0f, 0.0f);
	}

	m_ghostObject = std::make_unique<btPairCachingGhostObject>();
	m_ghostObject->setWorldTransform(create_bt_transform());
	Physics::Get_Broadphase().getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	m_ghostObject->setCollisionShape(m_shape.get());
	m_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	m_charCon = std::make_unique<btKinematicCharacterController>(m_ghostObject.get(), m_shape.get(), 0.05f, btVector3(0, 1, 0));
	m_charCon->setGravity(btVector3(0, Physics::Gravity(), 0));

	Physics::GetDynamicWorld().addCollisionObject(m_ghostObject.get(), btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter);
	Physics::GetDynamicWorld().addAction(m_charCon.get());
#else


	mSettings = new CharacterVirtualSettings();
	mSettings->mMaxSlopeAngle = sMaxSlopeAngle;
	mSettings->mMaxStrength = sMaxStrength;
	mSettings->mShape = mStandingShape;
	mSettings->mBackFaceMode = sBackFaceMode;
	mSettings->mCharacterPadding = sCharacterPadding;
	mSettings->mPenetrationRecoverySpeed = sPenetrationRecoverySpeed;
	mSettings->mPredictiveContactDistance = sPredictiveContactDistance;
	mSettings->mSupportingVolume =JPH::Plane(Vec3::sAxisY(), -cCharacterRadiusStanding); // Accept contacts that touch the lower sphere of the capsule
	mSettings->mEnhancedInternalEdgeRemoval = sEnhancedInternalEdgeRemoval;
	mSettings->mInnerBodyShape = sCreateInnerBody ? mInnerStandingShape : nullptr;
	mSettings->mInnerBodyLayer = Layers::MOVING;

	glm::vec3 pos = Object().Get_Transform().Position();
	glm::quat rot = Object().Get_Transform().Rotation();


	m_reset_pos = RVec3(pos.x, pos.y, pos.z);
	m_reset_character = true;

#endif

	Logger::LogDebug(LOG_POS("OnRefresh"), "Created character collider components.");
	//assert(false);
}


void CharacterCollider::HandleMovement(Vec3 move_vec, float dt)
{
	if (!m_has_character || mCharacter.GetPtr() == nullptr)
		return;

	bool player_controls_horizontal_velocity = sControlMovementDuringJump || mCharacter->IsSupported();
	if (player_controls_horizontal_velocity)
	{
		// Smooth the player input
		mDesiredVelocity = sEnableCharacterInertia ? 0.25f * move_vec * sCharacterSpeed + 0.75f * mDesiredVelocity : move_vec * sCharacterSpeed;

		// True if the player intended to move
		mAllowSliding = !move_vec.IsNearZero();
	}
	else
	{
		// While in air we allow sliding
		mAllowSliding = true;
	}

	// Update the character rotation and its up vector to match the up vector set by the user settings
	Quat character_up_rotation = Quat::sEulerAngles(Vec3(sUpRotationX, 0, sUpRotationZ));
	mCharacter->SetUp(character_up_rotation.RotateAxisY());
	mCharacter->SetRotation(character_up_rotation);

	// A cheaper way to update the character's ground velocity,
	// the platforms that the character is standing on may have changed velocity
	mCharacter->UpdateGroundVelocity();

	// Determine new basic velocity
	Vec3 current_vertical_velocity = mCharacter->GetLinearVelocity().Dot(mCharacter->GetUp()) * mCharacter->GetUp();
	Vec3 ground_velocity = mCharacter->GetGroundVelocity();
	Vec3 new_velocity;
	bool moving_towards_ground = (current_vertical_velocity.GetY() - ground_velocity.GetY()) < 0.1f;

	if (mCharacter->GetGroundState() == CharacterVirtual::EGroundState::OnGround	// If on ground
		&& (sEnableCharacterInertia ?
			moving_towards_ground													// Inertia enabled: And not moving away from ground
			: !mCharacter->IsSlopeTooSteep(mCharacter->GetGroundNormal())))			// Inertia disabled: And not on a slope that is too steep
	{
		// Assume velocity of ground when on ground
		new_velocity = ground_velocity;

		// Jump
		//if (inJump && moving_towards_ground)
		//	new_velocity += sJumpSpeed * mCharacter->GetUp();
	}
	else
		new_velocity = current_vertical_velocity;

	// Gravity
	new_velocity += (character_up_rotation * Physics::GetPhysicsSystem().GetGravity()) * dt;

	if (player_controls_horizontal_velocity)
	{
		// Player input
		new_velocity += character_up_rotation * mDesiredVelocity;
	}
	else
	{
		// Preserve horizontal velocity
		Vec3 current_horizontal_velocity = mCharacter->GetLinearVelocity() - current_vertical_velocity;
		new_velocity += current_horizontal_velocity;
	}

	// Update character velocity
	mCharacter->SetLinearVelocity(new_velocity);
	Vec3 loc = mCharacter->GetPosition();
	//mDesiredVelocity
	//Graphics::DrawDebugRay(Physics::glm_vec3(loc), Physics::glm_vec3(mDesiredVelocity), glm::vec3(1, 0, 0));
	//Graphics::DrawDebugRay(Physics::glm_vec3(loc), Physics::glm_vec3(new_velocity), glm::vec3(0, 1, 0));

}


