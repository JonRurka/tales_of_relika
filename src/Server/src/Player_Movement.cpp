#include "Player_Movement.h"

#include "Utilities.h"
#include "World.h"

Player_Movement::Player_Movement()
{
}

void Player_Movement::Set_Move_State(bool do_move, glm::vec2 move_dir)
{
	m_move_state.Do_Move = do_move;
	m_move_state.Move_Dir = move_dir;
}

void Player_Movement::Refresh()
{
	Remove();

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	m_shape = new btCapsuleShapeZ(m_radius, m_height);
	m_shape->calculateLocalInertia(m_mass, m_localInertia);

	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(m_location.x, m_location.y, m_location.z));

	m_ghostObject = new btPairCachingGhostObject();
	m_ghostObject->setWorldTransform(startTransform);
	m_ghostObject->setCollisionShape(m_shape);
	m_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	m_charCon = new btKinematicCharacterController(m_ghostObject, (btCapsuleShapeZ*)m_shape, 0.05f, btVector3(0, 1, 0));
	m_charCon->setGravity(btVector3(0, m_current_world->Physics()->Gravity(), 0));

	m_current_world->Physics()->GetDynamicWorld()->addCollisionObject(m_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter);
	m_current_world->Physics()->GetDynamicWorld()->addAction(m_charCon);
#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)

	mStandingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightStanding + cCharacterRadiusStanding, 0), Quat::sIdentity(), new CapsuleShape(0.5f * cCharacterHeightStanding, cCharacterRadiusStanding)).Create().Get();
	mCrouchingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightCrouching + cCharacterRadiusCrouching, 0), Quat::sIdentity(), new CapsuleShape(0.5f * cCharacterHeightCrouching, cCharacterRadiusCrouching)).Create().Get();
	mInnerStandingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightStanding + cCharacterRadiusStanding, 0), Quat::sIdentity(), new CapsuleShape(0.5f * cInnerShapeFraction * cCharacterHeightStanding, cInnerShapeFraction * cCharacterRadiusStanding)).Create().Get();
	mInnerCrouchingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightCrouching + cCharacterRadiusCrouching, 0), Quat::sIdentity(), new CapsuleShape(0.5f * cInnerShapeFraction * cCharacterHeightCrouching, cInnerShapeFraction * cCharacterRadiusCrouching)).Create().Get();

	mSettings = new CharacterVirtualSettings();
	mSettings->mMaxSlopeAngle = sMaxSlopeAngle;
	mSettings->mMaxStrength = sMaxStrength;
	mSettings->mShape = mStandingShape;
	mSettings->mBackFaceMode = sBackFaceMode;
	mSettings->mCharacterPadding = sCharacterPadding;
	mSettings->mPenetrationRecoverySpeed = sPenetrationRecoverySpeed;
	mSettings->mPredictiveContactDistance = sPredictiveContactDistance;
	mSettings->mSupportingVolume = Plane(Vec3::sAxisY(), -cCharacterRadiusStanding); // Accept contacts that touch the lower sphere of the capsule
	mSettings->mEnhancedInternalEdgeRemoval = sEnhancedInternalEdgeRemoval;
	mSettings->mInnerBodyShape = sCreateInnerBody ? mInnerStandingShape : nullptr;
	mSettings->mInnerBodyLayer = Layers::MOVING;

	assert(m_current_world != nullptr);

	mCharacter = new CharacterVirtual(mSettings, RVec3(m_position.x, m_position.y, m_position.z), Quat::sIdentity(), 0, &m_current_world->Physics()->GetPhysicsSystem());
	mCharacter->SetCharacterVsCharacterCollision(&mCharacterVsCharacterCollision);

#endif

	m_initialized = true;
}

void Player_Movement::Remove()
{
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	if (m_charCon != nullptr) {
		m_current_world->Physics()->GetDynamicWorld()->removeAction(m_charCon);
		m_current_world->Physics()->GetDynamicWorld()->removeCollisionObject(m_ghostObject);

		delete m_charCon;
		m_charCon = nullptr;

		delete m_ghostObject;
		m_ghostObject = nullptr;

		delete m_shape;
		m_shape = nullptr;
	}
#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)


#endif

	m_initialized = false;
}


#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
void Player_Movement::Jump()
{
	if (!m_initialized)
		return;
	m_charCon->jump(btVector3(0, m_jump_power, 0));
}

void Player_Movement::Update(float dt)
{
	if (!m_initialized)
		return;
	if (m_charCon == nullptr)
		return;

	if (m_move_state.Do_Move) {
		glm::vec3 tr_move_vec = glm::vec3(m_move_state.Move_Dir.x, 0, m_move_state.Move_Dir.y);

		if (m_charCon->onGround())
			m_charCon->setWalkDirection(btVector3(tr_move_vec.x, tr_move_vec.y, tr_move_vec.z).normalized() / 10);
		else
			m_charCon->setWalkDirection(btVector3(tr_move_vec.x, tr_move_vec.y, tr_move_vec.z).normalized() / 10);

	}
	else {
		m_charCon->setWalkDirection(btVector3(0, 0, 0));
	}

	btTransform t;
	t = m_charCon->getGhostObject()->getWorldTransform();
	btVector3 pos = t.getOrigin();
	btQuaternion quat = t.getRotation();
	m_position = glm::vec3(pos.x(), pos.y(), pos.z());

	btVector3 bt_vel = m_charCon->getLinearVelocity();
	m_velocity = glm::fvec3(bt_vel.x(), bt_vel.y(), bt_vel.z());// (m_location - m_old_location) / dt;
	//m_old_location = m_location;

	if (Utilities::Get_Time() - m_debug_timer > 1.0f)
	{
		//if (m_move_state.Do_Move)
		//	Logger::LogDebug(LOG_POS("move_control"), "SERVER MOVING");

		m_debug_timer = Utilities::Get_Time();

		//Logger::LogDebug(LOG_POS("move_control"), "Current Pos: (%f, %f, %f), Velocity: (%f, %f, %f)",
		//	m_location.x, m_location.y, m_location.z, 
		//	m_velocity.x, m_velocity.y, m_velocity.z);
	}
}

#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)

void Player_Movement::Jump()
{
	if (!m_initialized)
		return;
}

void Player_Movement::Update(float dt)
{
	if (!m_initialized)
		return;

	Vec3 move_vec = Vec3(m_move_state.Move_Dir.x, 0, m_move_state.Move_Dir.y);

	if (m_move_state.Do_Move)
		move_vec = Vec3(0, 0, 0);

	if (move_vec != Vec3::sZero()) {
		move_vec = move_vec.Normalized();
	}

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
	new_velocity += (character_up_rotation * m_current_world->Physics()->GetPhysicsSystem().GetGravity()) * dt;

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
}



#endif