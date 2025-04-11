#pragma once

#include "Physics.h"
#include "Component.h"

class Collider : public Component {
public:

	void Mass(float val) 
	{ 
		m_mass = val;
		OnUpdateMass(m_mass);
	}
	float Mass() { return m_mass; }

	bool Is_Dynamic() { return m_mass != 0.0f; }
	

private:

	btScalar m_mass{ 0.0 };

	btRigidBody* m_rigidbody{ nullptr };

	inline static const std::string LOG_LOC{ "COLLIDER" };

protected:

	void base_Init();
	void base_Update(float dt);

	btTransform get_bt_transform();

	void set_rigidbody(btRigidBody* body);

	virtual void Init() = 0;
	virtual void Update(float dt) = 0;
	virtual void Load(json data) = 0;

	virtual void OnUpdateMass(float mass) = 0;

	btVector3 m_localInertia{ btVector3(0.0f, 0.0f, 0.0f) };
};