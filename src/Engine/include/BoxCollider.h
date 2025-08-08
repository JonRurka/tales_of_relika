#pragma once

#include "Collider.h"

#define DEFAULT_SIZE (1.0f)

class BoxCollider : public Collider {
public:

	void Size(glm::vec3 size);

private:

	std::unique_ptr<btCollisionShape> m_shape{nullptr};
	

	inline static const std::string LOG_LOC{ "BOX_COLLIDER" };

protected:

	glm::vec3 m_size {glm::vec3(DEFAULT_SIZE, DEFAULT_SIZE, DEFAULT_SIZE)};

	void Init() override;
	void Update(float dt) override;
	void Load(json data) override;

	//void OnUpdateMass(float mass) override;
	void OnRefresh() override;
};

#undef DEFAULT_SIZE