#pragma once

#include "Collider.h"

#define DEFAULT_CAPSUE_RADIUS (0.5f)
#define DEFAULT_CAPSUE_HEIGHT (1.5f)

class btPairCachingGhostObject;
class btKinematicCharacterController;

class CharacterCollider : public Collider {
public:
	typedef std::shared_ptr<CharacterCollider> Shared;
	typedef std::weak_ptr<CharacterCollider> Weak;

	void Radius(float radius);

	void Height(float height);

	void Set_Location(glm::vec3 pos);

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	btPairCachingGhostObject& Get_Ghost_Object() { return *m_ghostObject.get(); }
	btKinematicCharacterController& Get_Controller() { return *m_charCon.get(); }

	void HandleMovement(Vec3 move_vec, float dt){}
#else
	CharacterVirtual& Get_Controller() { return *mCharacter.GetPtr(); }

	void Jump(float force);

	bool Character_Inited() { return m_has_character; }

	void HandleMovement(Vec3 move_vec, float dt);

	void ControllerEnabled(bool val);
	bool ControllerEnabled()
	{
		return m_contrl_enabled;
	}

#endif

private:

	bool m_has_character{ false };

	inline static const std::string LOG_LOC{ "CHARACTER_COLLIDER" };

protected:

	
	float m_height{ DEFAULT_CAPSUE_HEIGHT };
	bool m_contrl_enabled{ true };

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	std::unique_ptr<btPairCachingGhostObject> m_ghostObject;
	std::unique_ptr<btKinematicCharacterController> m_charCon;
	std::unique_ptr<btCapsuleShapeZ> m_shape{ nullptr };
	float m_radius{ DEFAULT_CAPSUE_RADIUS };

#else
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

	Vec3 m_pos;
	Quat m_rot;
	volatile bool m_reset_character{ false };
	RVec3 m_reset_pos;

	std::mutex m_lock;

#endif

	void Init() override;
	void Update(float dt) override;
	void FixedUpdate(float dt) override;
	void Load(json data) override;
	void OnDestroy() override;

	//void OnUpdateMass(float mass) override;
	void OnRefresh() override;
};

