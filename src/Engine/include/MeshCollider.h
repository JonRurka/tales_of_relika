#pragma once

#include "Collider.h"
#include "Mesh.h"

#define DEFAULT_SIZE (1.0f)

class Mesh;

class MeshCollider : public Collider {
public:
	typedef std::shared_ptr<MeshCollider> Shared;
	typedef std::weak_ptr<MeshCollider> Weak;

	void SetMesh(Mesh::Shared mesh);

private:

	std::unique_ptr<btCollisionShape> m_shape;
	std::unique_ptr<btTriangleIndexVertexArray> mTriangleIndexVertexArray;
	std::unique_ptr<btTriangleMesh> m_triangle_mesh;

	std::vector<glm::vec3> vert3;

	inline static const std::string LOG_LOC{ "MESH_COLLIDER" };

protected:

	std::shared_ptr<Mesh> m_mesh;

	unsigned int* m_indices{ nullptr };
	glm::vec3* m_vertices{ nullptr };

	void Init() override;
	void Update(float dt) override;
	void Load(json data) override;

	//void OnUpdateMass(float mass) override;
	void OnRefresh() override;

	void OnDestroy() override;
};

#undef DEFAULT_SIZE