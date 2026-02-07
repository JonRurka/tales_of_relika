#include "MeshCollider.h"

#include "WorldObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Logger.h"
#include "Utilities.h"
#include "Graphics.h"

#define DEFAULT_SIZE (1.0f)


int MeshCollider::s_num_threads{ 0 };

// Run on external thread
void MeshCollider::RunShapeWorker(ShapeBuildRequest* req)
{
	TriangleList triangles;
	for (int t_idx = 0; t_idx < req->verts.size() / 3; t_idx++)
	{
		glm::vec4 gv1 = req->verts[(t_idx * 3) + 0];
		glm::vec4 gv2 = req->verts[(t_idx * 3) + 1];
		glm::vec4 gv3 = req->verts[(t_idx * 3) + 2];

		Float3 v1 = Float3(gv1.x, gv1.y, gv1.z);
		Float3 v2 = Float3(gv2.x, gv2.y, gv2.z);
		Float3 v3 = Float3(gv3.x, gv3.y, gv3.z);

		triangles.push_back(Triangle(v1, v2, v3, 0));
	}

	PhysicsMaterialList materials;
	materials.push_back(new PhysicsMaterialSimple("Phy_Material", Color::sGetDistinctColor(0)));

	req->Shape_Settings = new MeshShapeSettings(triangles, std::move(materials));

	req->Ready = true;
	//Logger::LogDebug(LOG_POS("RunShapeWorker"), "Finished building shape.");
}


void MeshCollider::Init()
{
	base_Init();
}

void MeshCollider::SetMesh(Mesh::Shared mesh, bool activate)
{
	//m_mesh = mesh;
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	std::vector<glm::vec3> tmp_verts = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
	};
	std::vector<unsigned int> tmp_tris = {
		0, 1, 2
	};

	std::vector<unsigned int>& index = mesh->Indices();
	std::vector<glm::vec4>& vert = mesh->Vertices();

	if (vert.size() <= 0) {
		return;
	}

	//Logger::Log(LOG_POS("SetMesh"), "Set collision mesh with %i vertices and %i indices.", vert.size(), index.size());

	vert3 = Utilities::vec4_to_vec3_arr(vert);

	if (index.size() > 0) {
		btIndexedMesh indexedMesh;
		indexedMesh.m_numTriangles = mesh->Indices().size() / 3;
		indexedMesh.m_triangleIndexBase = reinterpret_cast<unsigned char*>(index.data());
		indexedMesh.m_triangleIndexStride = 3 * sizeof(unsigned int);
		indexedMesh.m_numVertices = mesh->Vertices().size();
		indexedMesh.m_vertexBase = reinterpret_cast<unsigned char*>(vert3.data());
		indexedMesh.m_vertexStride = sizeof(glm::vec3);

		mTriangleIndexVertexArray = std::make_unique<btTriangleIndexVertexArray>();
		mTriangleIndexVertexArray->addIndexedMesh(indexedMesh);

		m_shape = std::make_unique<btBvhTriangleMeshShape>(mTriangleIndexVertexArray.get(), true, true);
	}
	else {
		m_triangle_mesh = std::make_unique<btTriangleMesh>();
		//m_triangle_mesh->addIndexedMesh(indexedMesh);
		glm::vec3 col = glm::vec3(0.0, 0.0, 0.0);

		for (int t = 0; t < vert.size() / 3; t++) {

			glm::vec4 v1 = vert[(t * 3) + 0];
			glm::vec4 v2 = vert[(t * 3) + 1];
			glm::vec4 v3 = vert[(t * 3) + 2];

			m_triangle_mesh->addTriangle(
				btVector3(v1.x, v1.y, v1.z),
				btVector3(v2.x, v2.y, v2.z),
				btVector3(v3.x, v3.y, v3.z)
			);

			//Graphics::DrawDebugRay(v1, glm::vec3(0, 0.1, 0), col, 10000);
			//Graphics::DrawDebugRay(v2, glm::vec3(0, 0.1, 0), col, 10000);
			//Graphics::DrawDebugRay(v3, glm::vec3(0, 0.1, 0), col, 10000);

			//col.z += (float)t / (float)(vert.size() / 3);

		}
		m_shape = std::make_unique<btBvhTriangleMeshShape>(m_triangle_mesh.get(), true, true);
	}
#else


	/*TriangleList triangles;
	verts = mesh->Vertices();
	for (int t_idx = 0; t_idx < verts.size() / 3; t_idx++)
	{
		glm::vec4 gv1 = verts[(t_idx * 3) + 0];
		glm::vec4 gv2 = verts[(t_idx * 3) + 1];
		glm::vec4 gv3 = verts[(t_idx * 3) + 2];

		Float3 v1 = Float3(gv1.x, gv1.y, gv1.z);
		Float3 v2 = Float3(gv2.x, gv2.y, gv2.z);
		Float3 v3 = Float3(gv3.x, gv3.y, gv3.z);

		triangles.push_back(Triangle(v1, v2, v3, 0));
	}

	PhysicsMaterialList materials;
	materials.push_back(new PhysicsMaterialSimple("Phy_Material", Color::sGetDistinctColor(0)));

	m_shape_settings = new MeshShapeSettings(triangles, std::move(materials));*/

#endif

	
	//if (activate)
	//	Activate();

	Clear();

	if (s_num_threads < MAX_SHAPE_THREADS)
	{
		add_build_request(mesh->Vertices());
	}
	else 
	{
		m_request_backlog.push(mesh->Vertices());
	}
}

void MeshCollider::add_build_request(const std::vector<glm::vec4>& req_verts)
{
	ShapeBuildRequest req;
	req.Ready = false;
	req.verts = req_verts;
	req.ID = ++m_latest_build_req_id;
	req.MColl = this;

	m_build_requests[req.ID] = req;
	m_build_requests_threads[req.ID] = std::thread(RunShapeWorker, &m_build_requests[req.ID]);
	s_num_threads++;
}

void MeshCollider::Update(float dt)
{
	base_Update(dt);

	bool should_activate = false;
	std::vector<uint64_t> to_remove;
	for (const auto& pair : m_build_requests)
	{
		if (pair.first < m_latest_build_req_id && pair.second.Ready)
		{
			//Logger::LogDebug(LOG_POS("Update"), "Disgard old shape.");
			to_remove.push_back(pair.second.ID);
		}
		else if (pair.first == m_latest_build_req_id && pair.second.Ready)
		{
			//Logger::LogDebug(LOG_POS("Update"), "Shape ready.");
			m_shape_settings = pair.second.Shape_Settings;
			to_remove.push_back(pair.second.ID);
			should_activate = true;
		}
	}

	for (const auto& id : to_remove)
	{
		s_num_threads--;
		m_build_requests_threads[id].join();
		m_build_requests_threads.erase(id);
		m_build_requests.erase(id);
	}

	// Add request from backlog
	while (!m_request_backlog.empty() && s_num_threads < MAX_SHAPE_THREADS)
	{
		add_build_request(m_request_backlog.front());
		m_request_backlog.pop();
	}

	if (should_activate)
	{
		//Logger::LogDebug(LOG_POS("Update"), "Activating collision mesh...");
		Activate();
		gen_rigidbody();
	}


	/*Transform* obj_trans = Object()->Get_Transform();
	btTransform bt_trans = get_bt_rigid_transform();
	Logger::LogDebug(LOG_POS("Update"), "Rigid Position:(%f, %f, %f), Obj Position:(%f, %f, %f)",
		bt_trans.getOrigin().x(), bt_trans.getOrigin().y(), bt_trans.getOrigin().z(),
		obj_trans->Position().x, obj_trans->Position().y, obj_trans->Position().z);*/
}

void MeshCollider::FixedUpdate(float dt)
{
	base_FixedUpdate(dt);

	//Logger::LogDebug(LOG_POS("FixedUpdate"), "call fixed update.");
}

void MeshCollider::Load(json data)
{
}

void MeshCollider::OnEnabled()
{
	base_OnEnabled();
}

void MeshCollider::OnDisabled()
{
	base_OnDisabled();
}

void MeshCollider::OnRefresh()
{
	if (!Active())
		return;
	/*if (Has_Rigidbody()) {
		remove_rigidbody();
	}*/

	


}

void MeshCollider::gen_rigidbody()
{
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)


	if (Is_Dynamic()) {
		m_shape->calculateLocalInertia(Mass(), m_localInertia);
	}
	else {
		m_localInertia = btVector3(0.0f, 0.0f, 0.0f);
	}

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	m_motionState = std::make_unique<btDefaultMotionState>(create_bt_transform());
	btRigidBody::btRigidBodyConstructionInfo rbInfo(Mass(), m_motionState.get(), m_shape.get(), m_localInertia);
	set_rigidbody(std::make_shared<btRigidBody>(rbInfo));

	//Transform* obj_trans = Object().Get_Transform();
	//btTransform bt_trans = get_bt_rigid_transform();
	//Logger::LogDebug(LOG_POS("OnRefresh"), "Rigid Position:(%f, %f, %f), Obj Position:(%f, %f, %f)",
	//	bt_trans.getOrigin().x(), bt_trans.getOrigin().y(), bt_trans.getOrigin().z(),
	//	obj_trans->Position().x, obj_trans->Position().y, obj_trans->Position().z);
#else

	assert(m_shape_settings != nullptr);

	glm::vec3 t_pos = Object().Get_Transform().Position();
	glm::quat t_rot = Object().Get_Transform().Rotation();
	RVec3 r_pos = RVec3(t_pos.x, t_pos.y, t_pos.z);
	Quat r_quat = Quat::sIdentity();
	bool normalizsed = r_quat.IsNormalized();
	BodyCreationSettings create_settings = BodyCreationSettings(m_shape_settings, r_pos, r_quat, EMotionType::Static, Layers::NON_MOVING);
	create_Rigidbody(create_settings);


	//Body* rigidbody = Physics::GetBodyInterface().CreateBody(BodyCreationSettings(m_shape_settings, r_pos, r_quat, EMotionType::Static, Layers::NON_MOVING));
	//Physics::GetBodyInterface().AddBody(rigidbody->GetID(), EActivation::Activate);
	//set_rigidbody(rigidbody);

	//Logger::LogDebug(LOG_POS("OnRefresh"), "Set rigidbody: %d", rigidbody->GetID());

#endif

}


void MeshCollider::Clear()
{
#if (PHYSICS_BACKEND==PHYSICS_BACKEND_BULLET)
	if (Has_Rigidbody()) {
		remove_rigidbody();
	}

	Destroy_Collider();

	m_triangle_mesh.reset();
	mTriangleIndexVertexArray.reset();
	m_shape.reset();
#else
	
	if (Has_Rigidbody()) {
		//Logger::LogDebug(LOG_POS("Destroy_Collider"), "Remove rigidbody");
		remove_rigidbody();
	}

#endif
}

void MeshCollider::OnDestroy()
{
	Clear();
	Destroy_Collider();
}
