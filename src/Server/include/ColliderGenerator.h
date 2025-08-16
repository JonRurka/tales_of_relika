#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include "stdafx.h"

#include "WorldPhysics.h"

class btTriangleMesh;
class btBvhTriangleMeshShape;

class ColliderGenerator {
public:

	struct Request {
		friend class ColliderGenerator;
	public:
		Request(glm::ivec3 loc, std::vector<glm::fvec4> verts, std::vector<unsigned int> tris) : 
			m_chunk_loc{ loc }, 
			m_vertices{ verts },
			m_triangles{ tris }
		{ }

		int Num_Verts() { return m_vertices.size(); }
		int Num_Tris() { return m_triangles.size(); }
		bool Ready() { return m_is_ready; }
		bool Valid() { return m_is_valid; }

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
		btTriangleMesh* Triangle_Mesh() { return m_triangle_mesh; }
		btBvhTriangleMeshShape* Mesh_Shape() { return m_mesh_shape; }

#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)
		Ref<MeshShapeSettings> Mesh_Shape() { return m_shape_settings; }
#endif

	private:
		glm::ivec3 m_chunk_loc{ glm::ivec3() };
		std::vector<glm::fvec4> m_vertices;
		std::vector<unsigned int> m_triangles;
		bool m_is_ready{false};
		bool m_is_valid{false};

#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
		btTriangleMesh* m_triangle_mesh{ nullptr };
		btBvhTriangleMeshShape* m_mesh_shape{ nullptr };
#elif (PHYSICS_BACKEND==PHYSICS_BACKEND_JOLT)

		Ref<MeshShapeSettings> m_shape_settings;

#endif
	};

	ColliderGenerator();

	void Init(int num_workers);

	static Request* Push_Request(glm::ivec3 loc, std::vector<glm::fvec4> verts, std::vector<unsigned int> tris)
	{
		return m_instance->push_request(loc, verts, tris);
	}

	static void Release_Request(Request* req) 
	{
		m_instance->release_request(req);
	}

	void Stop();

private:

	static ColliderGenerator* m_instance;

	std::queue<Request*> m_request_queue;
	std::vector<std::thread> m_thread_workers;
	std::mutex m_lock;
	
	int m_num_workers{ 0 };
	bool m_running{ false };

	Request* push_request(glm::ivec3 loc, std::vector<glm::fvec4> verts, std::vector<unsigned int> tris);

	void release_request(Request* req);
	
	static void Create_Worker();
	void run_worker();

	void do_process();

	void process_request(Request* req);

	inline static const std::string LOG_LOC{ "SERVER_COLLIDER_GENERATOR" };

};