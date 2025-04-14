#include "MeshCollider.h"

#include "WorldObject.h"
#include "Transform.h"
#include "Mesh.h"

#define DEFAULT_SIZE (1.0f)

void MeshCollider::Init()
{
	base_Init();

	
}

void MeshCollider::SetMesh(Mesh* mesh)
{
	m_mesh = mesh;

	std::vector<unsigned int>& index = mesh->Indices();
	std::vector<glm::vec3>& vert = mesh->Vertices();

	btIndexedMesh indexedMesh;
	indexedMesh.m_numTriangles = mesh->Indices().size() / 3;
	indexedMesh.m_triangleIndexBase = (unsigned char*)index.data();
	indexedMesh.m_triangleIndexStride = 3 * sizeof(unsigned int);
	indexedMesh.m_numVertices = mesh->Vertices().size();
	indexedMesh.m_vertexBase = (unsigned char*)vert.data();
	indexedMesh.m_vertexStride = sizeof(glm::vec3);

	mTriangleIndexVertexArray = new btTriangleIndexVertexArray();
	mTriangleIndexVertexArray->addIndexedMesh(indexedMesh);

	m_shape = new btBvhTriangleMeshShape(mTriangleIndexVertexArray, true, true);
	
	OnRefresh();
}

void MeshCollider::Update(float dt)
{
	base_Update(dt);
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
	btDefaultMotionState* myMotionState = new btDefaultMotionState(get_bt_transform());
	btRigidBody::btRigidBodyConstructionInfo rbInfo(Mass(), myMotionState, m_shape, m_localInertia);
	set_rigidbody(new btRigidBody(rbInfo));
}