#pragma once

#include <unordered_map>
#include <queue>

#include "Collider.h"
#include "Mesh.h"

#define DEFAULT_SIZE (1.0f)
#define MAX_SHAPE_THREADS 10

class Mesh;



class MeshCollider : public Collider {
public:
	typedef std::shared_ptr<MeshCollider> Shared;
	typedef std::weak_ptr<MeshCollider> Weak;

	struct ShapeBuildRequest
	{
	public:
		std::vector<glm::vec4> verts;
		Ref<MeshShapeSettings> Shape_Settings;
		MeshCollider* MColl{ nullptr };
		bool Ready{ false };
		uint64_t ID{ 0 };

		ShapeBuildRequest() = default;

	};
	
	void SetMesh(Mesh::Shared mesh, bool activate = true);

	void Clear();

private:

	

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	std::unique_ptr<btCollisionShape> m_shape;
	std::unique_ptr<btTriangleIndexVertexArray> mTriangleIndexVertexArray;
	std::unique_ptr<btTriangleMesh> m_triangle_mesh;
	std::unique_ptr<btDefaultMotionState> m_motionState;
#else
	Ref<MeshShapeSettings> m_shape_settings;

#endif



	std::vector<glm::vec3> vert3;


	std::queue<std::vector<glm::vec4>> m_request_backlog;
	std::unordered_map<uint64_t, ShapeBuildRequest> m_build_requests;
	std::unordered_map<uint64_t, std::thread> m_build_requests_threads;

	static int s_num_threads;

	uint64_t m_latest_build_req_id{0};

	void add_build_request(const std::vector<glm::vec4>& req_verts);

	
	static void RunShapeWorker(ShapeBuildRequest* req);

	void gen_rigidbody();


	inline static const std::string LOG_LOC{ "MESH_COLLIDER" };


protected:

	std::shared_ptr<Mesh> m_mesh;

	void Init() override;
	void Update(float dt) override;
	void FixedUpdate(float dt) override;
	void Load(json data) override;

	//void OnUpdateMass(float mass) override;
	void OnRefresh() override;

	void OnDestroy() override;
};

#undef DEFAULT_SIZE