#include "MeshRenderer.h"

#include "WorldObject.h"
#include "Mesh.h";
#include "Shader.h";
#include "Material.h"
#include "Transform.h"
#include "Camera.h"
#include "Graphics.h"
#include "Logger.h"

MeshRenderer::MeshRenderer(WorldObject* obj)
{
	m_object = obj;
	worldObject(obj);
	m_mesh = new Mesh();
	m_mesh->Name("null");
	m_shader = nullptr;
	has_shader = false;
}

void MeshRenderer::Update(float dt)
{
	//update_model_matrix();
}

void MeshRenderer::update_model_matrix()
{

	if (m_bound_material) {
		m_bound_material->Get_Shader()->setMat4x4("model_mat", m_object->Get_Transform()->Get_Model_Matrix4());
		m_bound_material->Get_Shader()->setMat3x3("normal_mat", m_object->Get_Transform()->Get_Normal_Matrix3());
	}
	else if (m_shader) {
		//Use();
		m_shader->setMat4x4("model_mat", m_object->Get_Transform()->Get_Model_Matrix4());
		m_shader->setMat3x3("normal_mat", m_object->Get_Transform()->Get_Normal_Matrix3());
		//m_shader->setMat4x4("projection", Camera::Get_Active()->Projection_Matrix());
		//m_shader->setMat4x4("view", Camera::Get_Active()->View_Matrix());
	}
}

void MeshRenderer::Set_Shader(Shader* shader)
{
	m_shader = shader;
	if (m_shader != nullptr) {
		has_shader = true;
		m_shader->Register_Renderer(this);
	}
}

void MeshRenderer::Set_Material(Material* material)
{
	if (material->Get_Shader() == nullptr) {
		Logger::LogError(LOG_POS("Set_Material"), "Attempt to set material with invalid shader.");
		return;
	}

	m_source_material = material;
	if (m_source_material != nullptr) {
		has_material = true;

		m_source_material->Get_Shader()->Register_Renderer(this);

		m_bound_material = m_source_material->Copy();
		m_bound_material->Name(m_source_material->Name() + "_clone");
		m_bound_material->Set_Shader(m_source_material->Get_Shader());
		m_bound_material->Set_World_Object(m_object);
		m_bound_material->m_source_material = m_source_material;
		m_source_material->Register_Renderer_Material(this, m_bound_material);
	}
}

void MeshRenderer::Set_Mesh(Mesh* value)
{
	if (value == nullptr) {
		return;
	}
	if (has_default_mesh) {
		delete m_mesh;
	}
	m_mesh = value;
	m_mesh->Activate();
	has_default_mesh = false;
}

void MeshRenderer::Use()
{
	if (has_shader) {
		m_shader->use();
	}
}

void MeshRenderer::Draw(float dt)
{
	update_model_matrix();
	m_source_material->Internal_Update(dt, true);
	m_bound_material->Internal_Update(dt);

	bool enable_transparency = m_bound_material->Transparent();// && !graphics->Active_Options().transparency_enabled;

	if (enable_transparency) {
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	m_mesh->Draw();

	if (enable_transparency) {
		//glDisable(GL_BLEND);
	}
}


