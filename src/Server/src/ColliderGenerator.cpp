#include "ColliderGenerator.h"

#include "Utilities.h"
#include "WorldPhysics.h"
#include "Logger.h"

ColliderGenerator* ColliderGenerator::m_instance{nullptr};

ColliderGenerator::ColliderGenerator()
{
	m_instance = this;
}

void ColliderGenerator::Init(int num_workers)
{
	m_num_workers = num_workers;
	m_running = true;
	m_thread_workers.reserve(num_workers);
	for (int i = 0; i < num_workers; i++)
	{
		m_thread_workers.push_back(std::thread(Create_Worker));
	}
}

void ColliderGenerator::Stop()
{
	m_running = false;
	for (int i = 0; i < m_num_workers; i++)
	{
		m_thread_workers[i].join();
	}
}

ColliderGenerator::Request* ColliderGenerator::push_request(glm::ivec3 loc, std::vector<glm::fvec4> verts, std::vector<unsigned int> tris)
{
	ColliderGenerator::Request* res = new ColliderGenerator::Request(loc, verts, tris);
	m_lock.lock();
	m_request_queue.push(res);
	m_lock.unlock();

	//Logger::LogInfo(LOG_POS("push_request"), "Push collider request.");

	return res;
}

void ColliderGenerator::release_request(Request* req)
{
	if (req == nullptr)
		return;

	if (req->m_mesh_shape != nullptr)
		delete req->m_mesh_shape;

	if (req->m_triangle_mesh != nullptr)
		delete req->m_triangle_mesh;

	delete req;
}

void ColliderGenerator::Create_Worker()
{
	m_instance->run_worker();
}

void ColliderGenerator::run_worker()
{
	Logger::LogInfo(LOG_POS("run_worker"), "Created collider generator worker.");

	while (m_running) 
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		do_process();
	}
}

void ColliderGenerator::do_process()
{
	bool has_next = false;
	do 
	{
		m_lock.lock();
		if (m_request_queue.empty()) {
			m_lock.unlock();
			break;
		}
		ColliderGenerator::Request* req = m_request_queue.front();
		m_request_queue.pop();
		m_lock.unlock();

		process_request(req);

		m_lock.lock();
		has_next = !m_request_queue.empty();
		m_lock.unlock();

	} while (has_next);
}

void ColliderGenerator::process_request(Request* req)
{
	if (req->m_vertices.size() <= 0) {
		req->m_is_ready = true;
		req->m_is_valid = false;
		return;
	}

	std::vector<glm::vec4> vert = req->m_vertices;
	std::vector<unsigned int> tris = req->m_triangles;


	if (tris.size() > 0) {
		std::vector<glm::vec3> vert3 = Utilities::vec4_to_vec3_arr(vert);

		btIndexedMesh indexedMesh;
		indexedMesh.m_numTriangles = tris.size() / 3;
		indexedMesh.m_triangleIndexBase = reinterpret_cast<unsigned char*>(tris.data());
		indexedMesh.m_triangleIndexStride = 3 * sizeof(unsigned int);
		indexedMesh.m_numVertices = vert.size();
		indexedMesh.m_vertexBase = reinterpret_cast<unsigned char*>(vert3.data());
		indexedMesh.m_vertexStride = sizeof(glm::vec3);

		btTriangleIndexVertexArray* m_opaque_TriangleIndexVertexArray = new btTriangleIndexVertexArray();
		m_opaque_TriangleIndexVertexArray->addIndexedMesh(indexedMesh);

		req->m_mesh_shape = new btBvhTriangleMeshShape(m_opaque_TriangleIndexVertexArray, true, true);
		//Logger::LogDebug(LOG_POS("set_opaque_collider"), "Created (index) collider with %i verts", vert.size());
	}
	else {

		auto start = std::chrono::high_resolution_clock::now();

		btTriangleMesh* m_opaque_triangle_mesh = new btTriangleMesh();
		//m_triangle_mesh->addIndexedMesh(indexedMesh);
		glm::vec3 col = glm::vec3(0.0, 0.0, 0.0);

		for (int t = 0; t < vert.size() / 3; t++) {

			glm::vec4 v1 = vert[(t * 3) + 0];
			glm::vec4 v2 = vert[(t * 3) + 1];
			glm::vec4 v3 = vert[(t * 3) + 2];

			m_opaque_triangle_mesh->addTriangle(
				btVector3(v1.x, v1.y, v1.z),
				btVector3(v2.x, v2.y, v2.z),
				btVector3(v3.x, v3.y, v3.z)
			);
		}
		req->m_triangle_mesh = m_opaque_triangle_mesh;
		req->m_mesh_shape = new btBvhTriangleMeshShape(m_opaque_triangle_mesh, true, true);

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration<double>(end - start).count();

		//Logger::LogDebug(LOG_POS("set_opaque_collider"), "Created collider with %i verts in %f ms",
		//	vert.size(), (float)((duration) * 1000.0f));
	}

	req->m_is_valid = true;
	req->m_is_ready = true;
	//Logger::LogInfo(LOG_POS("process_request"), "process collider with %i verts.", req->m_vertices.size());
}


