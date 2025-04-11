#pragma once

#include "Collider.h"

class BoxCollider : public Collider {
public:

private:

	btCollisionShape* m_shape{nullptr};
	

	inline static const std::string LOG_LOC{ "BOX_COLLIDER" };

protected:

	void Init() override;
	void Update(float dt) override;
	void Load(json data) override;

	void OnUpdateMass(float mass) override;
};