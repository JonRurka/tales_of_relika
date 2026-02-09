#pragma once

#include <memory>

#include "Physics.h"
#include "Component.h"

class Collider : public Component 
{
	friend class Physics;
public:
	typedef std::shared_ptr<Collider> Shared;
	typedef std::weak_ptr<Collider> Weak;

	void Mass(float val) 
	{ 
		m_mass = val;
		if (m_active)
			OnRefresh();
	}
	float Mass() 
	{ 
		return m_mass; 
	}

	void Activate() {
		m_active = true;
		OnRefresh();
	}
	
	bool Active() {
		return m_active;
	}

	bool Is_Dynamic() { return m_mass > 0.0001f; }
	
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	btRigidBody& RigidBody() { return *m_rigidbody.get(); }
	std::weak_ptr<btRigidBody> RigidBody_Ptr() { return m_rigidbody; }
	bool Has_Rigidbody() { return m_rigidbody.get() != nullptr; }
#else
	Body& Rigidbody() { return *m_rigidbody; }
	Body* Rigidbody_Ptr() { return m_rigidbody; }
	bool Has_Rigidbody() { return m_has_rigidbody && m_rigidbody != nullptr; }

	void SetUserData(uint64_t val) {
		m_user_data = val;
		if (m_rigidbody != nullptr) {
			m_rigidbody->SetUserData(m_user_data);
		}
	}

	uint64_t GetUserData() {
		return m_user_data;
	}

#endif

	

private:

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	btScalar m_mass{ 0.0 };
	std::shared_ptr<btRigidBody> m_rigidbody{ nullptr };
	btVector3 m_localInertia{ btVector3(0.0f, 0.0f, 0.0f) };
#else
	float m_mass{ 0.0 };
	Body* m_rigidbody{ nullptr };
	volatile bool m_has_rigidbody{ false };
	glm::vec3 m_pos;
	glm::quat m_rot;
	uint32_t m_col_id{ 0 };
#endif

	uint64_t m_user_data{ 0 };
	std::mutex m_lock;

	bool m_active{ false };

	void OnSetRigidbody(Body* body) // TODO Callback from physics engine with rigidbody.
	{
		m_rigidbody = body;
		m_rigidbody->SetUserData(m_user_data);
		m_has_rigidbody = true;
	}

	inline static const std::string LOG_LOC{ "COLLIDER" };

protected:

	void base_Init();
	void base_Update(float dt);
	void base_FixedUpdate(float dt);
	void base_OnEnabled();
	void base_OnDisabled();

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	btTransform create_bt_transform();
	btTransform get_bt_rigid_transform();

	void set_rigidbody(std::shared_ptr<btRigidBody> body);
	void remove_rigidbody();
#else
	void create_Rigidbody(BodyCreationSettings settings); // TODO from shape. Request physics engine to create it via queue

	// Depricated.
	void set_rigidbody(Body* body);


	void remove_rigidbody();

	

#endif


	

	virtual void Init() = 0;
	virtual void Update(float dt) = 0;
	virtual void Load(json data) = 0;

	//virtual void OnUpdateMass(float mass) = 0;
	virtual void OnRefresh() = 0;

	void Destroy_Collider();

	
};