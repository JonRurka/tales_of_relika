#include "MeshCollider.h"

#include "WorldObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Logger.h"
#include "Utilities.h"

#define DEFAULT_SIZE (1.0f)

void MeshCollider::Init()
{
	base_Init();

	
}

void MeshCollider::SetMesh(Mesh* mesh)
{
	//m_mesh = mesh;

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

	Logger::Log(LOG_POS("SetMesh"), "Set collision mesh with %i vertices and %i indices.", vert.size(), index.size());

	std::vector<glm::vec3> vert3 = Utilities::vec4_to_vec3_arr(vert);

	btIndexedMesh indexedMesh;
	indexedMesh.m_numTriangles = mesh->Indices().size() / 3;
	indexedMesh.m_triangleIndexBase = (unsigned char*)index.data();
	indexedMesh.m_triangleIndexStride = 3 * sizeof(unsigned int);
	indexedMesh.m_numVertices = mesh->Vertices().size();
	indexedMesh.m_vertexBase = (unsigned char*)vert3.data();
	indexedMesh.m_vertexStride = sizeof(glm::vec3);

	mTriangleIndexVertexArray = new btTriangleIndexVertexArray();
	mTriangleIndexVertexArray->addIndexedMesh(indexedMesh);

	//m_triangle_mesh = new btTriangleMesh();
	//m_triangle_mesh->addIndexedMesh(indexedMesh);
	/*for (int t = 0; t < mesh->Indices().size() / 3; t++) {

		glm::vec4 v1 = vert[(t * 3) + 0];
		glm::vec4 v2 = vert[(t * 3) + 1];
		glm::vec4 v3 = vert[(t * 3) + 2];

		m_triangle_mesh->addTriangle(
			btVector3(v1.x, v1.y, v1.z),
			btVector3(v2.x, v2.y, v2.z),
			btVector3(v3.x, v3.y, v3.z)
		);
	}*/

	m_shape = new btBvhTriangleMeshShape(mTriangleIndexVertexArray, true, true);
	
	OnRefresh();
}

void MeshCollider::Update(float dt)
{
	base_Update(dt);

	/*Transform* obj_trans = Object()->Get_Transform();
	btTransform bt_trans = get_bt_rigid_transform();
	Logger::LogDebug(LOG_POS("Update"), "Rigid Position:(%f, %f, %f), Obj Position:(%f, %f, %f)",
		bt_trans.getOrigin().x(), bt_trans.getOrigin().y(), bt_trans.getOrigin().z(),
		obj_trans->Position().x, obj_trans->Position().y, obj_trans->Position().z);*/
}

void MeshCollider::Load(json data)
{
}

void MeshCollider::OnRefresh()
{
	if (!Active())
		return;

	if (RigidBody() != nullptr) {
		remove_rigidbody(RigidBody());
	}

	if (Is_Dynamic()) {
		m_shape->calculateLocalInertia(Mass(), m_localInertia);
	}
	else {
		m_localInertia = btVector3(0.0f, 0.0f, 0.0f);
	}

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(create_bt_transform());
	btRigidBody::btRigidBodyConstructionInfo rbInfo(Mass(), myMotionState, m_shape, m_localInertia);
	set_rigidbody(new btRigidBody(rbInfo));

	Transform* obj_trans = Object()->Get_Transform();
	btTransform bt_trans = get_bt_rigid_transform();
	Logger::LogDebug(LOG_POS("OnRefresh"), "Rigid Position:(%f, %f, %f), Obj Position:(%f, %f, %f)",
		bt_trans.getOrigin().x(), bt_trans.getOrigin().y(), bt_trans.getOrigin().z(),
		obj_trans->Position().x, obj_trans->Position().y, obj_trans->Position().z);
}