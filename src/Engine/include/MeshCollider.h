#pragma once

#include "Collider.h"

#define DEFAULT_SIZE (1.0f)

class Mesh;

class MeshCollider : public Collider {
public:

	void SetMesh(Mesh* mesh);

private:

	btCollisionShape* m_shape{ nullptr };
	btTriangleIndexVertexArray* mTriangleIndexVertexArray{ nullptr };

	inline static const std::string LOG_LOC{ "MESH_COLLIDER" };

protected:

	Mesh* m_mesh{ nullptr };

	unsigned int* m_indices{ nullptr };
	glm::vec3* m_vertices{ nullptr };

	void Init() override;
	void Update(float dt) override;
	void Load(json data) override;

	//void OnUpdateMass(float mass) override;
	void OnRefresh() override;
};

#undef DEFAULT_SIZE