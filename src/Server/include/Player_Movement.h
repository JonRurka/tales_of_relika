#pragma once

#include "WorldPhysics.h"

class btPairCachingGhostObject;
class btKinematicCharacterController;

#define DEFAULT_RADIUS (0.5f)
#define DEFAULT_HEIGHT (1.5f)
#define DEFAULT_MASS (50.0f)
#define JUMP_POWER_DEFAULT (6.0)

#define DEFAULT_CAPSUE_RADIUS (0.5f)
#define DEFAULT_CAPSUE_HEIGHT (1.5f)

class World;

class Player_Movement {

public:

	struct PlayerMoveState {
	public:
		bool Do_Move{ false };
		glm::vec2 Move_Dir{ glm::vec2(0, 0) };
	};
	
	void Position(glm::vec3 val) { m_position = val; }
	glm::vec3 Position() { return m_position; }

	glm::vec3 Velocity() { return m_velocity; }

	void Current_World(World* val) { m_current_world = val; }

	bool Initialized() { return m_initialized; }

	Player_Movement();

	void Set_Move_State(bool do_move, glm::vec2 move_dir);

	void Refresh();

	void Remove();

	void Jump();

	void Update(float dt);

private:

	glm::vec3 m_position;
	glm::vec3 m_velocity;
	double m_debug_timer{ 0 };
	PlayerMoveState m_move_state{};
	World* m_current_world{ nullptr };
	bool m_initialized{ false };
	float m_jump_force{ 5.0f };

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	btCollisionShape* m_shape{ nullptr };
	float m_radius{ DEFAULT_RADIUS };
	float m_height{ DEFAULT_HEIGHT };
	btPairCachingGhostObject* m_ghostObject{ nullptr };
	btKinematicCharacterController* m_charCon{ nullptr };
	btVector3 m_localInertia{ btVector3(0.0f, 0.0f, 0.0f) };
	float m_mass{ DEFAULT_MASS };
	float m_jump_power{ JUMP_POWER_DEFAULT };


#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)

	Ref<CharacterVirtualSettings> mSettings;
	Ref<CharacterVirtual> mCharacter;

	// List of active characters in the scene so they can collide
	CharacterVsCharacterCollisionSimple mCharacterVsCharacterCollision;

	RefConst<Shape>	mStandingShape;
	RefConst<Shape>	mCrouchingShape;
	RefConst<Shape>	mInnerCrouchingShape;
	RefConst<Shape>	mInnerStandingShape;

	float sMaxSlopeAngle = DegreesToRadians(45.0f);
	float sMaxStrength = 100.0f;
	EBackFaceMode sBackFaceMode = EBackFaceMode::CollideWithBackFaces;
	float sCharacterPadding = 0.02f;
	float sPenetrationRecoverySpeed = 1.0f;
	float sPredictiveContactDistance = 0.1f;
	bool sEnhancedInternalEdgeRemoval = false;
	float		sUpRotationX = 0;
	float		sUpRotationZ = 0;

	bool sControlMovementDuringJump = true;
	bool sEnableCharacterInertia = true;
	float sCharacterSpeed = 6.0f;
	float sJumpSpeed = 4.0f;

	float cCharacterHeightStanding = 1.35f;
	float cCharacterRadiusStanding = DEFAULT_CAPSUE_RADIUS;
	float cCharacterHeightCrouching = 0.8f;
	float cCharacterRadiusCrouching = DEFAULT_CAPSUE_RADIUS;
	float cInnerShapeFraction = 0.9f;

	bool sCreateInnerBody = false;

	// Smoothed value of the player input
	Vec3 mDesiredVelocity = Vec3::sZero();

	// True when the player is pressing movement controls
	bool mAllowSliding = false;

	inline static const std::string LOG_LOC{ "SERVER_PLAYER_MOVEMENT" };

#endif
};