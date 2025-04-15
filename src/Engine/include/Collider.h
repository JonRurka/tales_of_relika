#pragma once

#include "Physics.h"
#include "Component.h"

class Collider : public Component {
public:

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
	
	btRigidBody* RigidBody() { return m_rigidbody; }

private:

	btScalar m_mass{ 0.0 };
	bool m_active{ false };

	btRigidBody* m_rigidbody{ nullptr };

	inline static const std::string LOG_LOC{ "COLLIDER" };

protected:

	void base_Init();
	void base_Update(float dt);

	btTransform create_bt_transform();

	btTransform get_bt_rigid_transform();


	void set_rigidbody(btRigidBody* body);
	void remove_rigidbody(btRigidBody* body);

	virtual void Init() = 0;
	virtual void Update(float dt) = 0;
	virtual void Load(json data) = 0;

	//virtual void OnUpdateMass(float mass) = 0;
	virtual void OnRefresh() = 0;


	btVector3 m_localInertia{ btVector3(0.0f, 0.0f, 0.0f) };
};