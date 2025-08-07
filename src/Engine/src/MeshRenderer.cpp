#include "MeshRenderer.h"

#include "WorldObject.h"
#include "Mesh.h";
#include "Shader.h";
#include "Material.h"
#include "Transform.h"
#include "Camera.h"
#include "Graphics.h"
#include "Logger.h"

MeshRenderer::MeshRenderer(std::weak_ptr<WorldObject> obj)
{
	m_object = obj;
	set_worldObject(obj);
	m_mesh = std::make_shared<Mesh>();
	m_mesh->Name("null");
	m_shader = nullptr;
	has_shader = false;
}

MeshRenderer::~MeshRenderer()
{
	Destroy();
}

void MeshRenderer::Update(float dt)
{
	//update_model_matrix();
}

void MeshRenderer::OnDestroy()
{
	if (has_default_mesh) {
		//delete m_mesh;
	}

	if (has_material && m_source_material != nullptr)
	{
		m_source_material->Remove_Renderer_Material(m_bound_material);
	}

}

void MeshRenderer::update_model_matrix()
{
	assert(!m_object.expired());

	if (m_bound_material) {
		m_bound_material->Get_Shader().setMat4x4("model_mat", m_object.lock()->Get_Transform().Get_Model_Matrix4());
		m_bound_material->Get_Shader().setMat3x3("normal_mat", m_object.lock()->Get_Transform().Get_Normal_Matrix3());
	}
	else if (m_shader) {
		//Use();
		m_shader->setMat4x4("model_mat", m_object.lock()->Get_Transform().Get_Model_Matrix4());
		m_shader->setMat3x3("normal_mat", m_object.lock()->Get_Transform().Get_Normal_Matrix3());
		//m_shader->setMat4x4("projection", Camera::Get_Active()->Projection_Matrix());
		//m_shader->setMat4x4("view", Camera::Get_Active()->View_Matrix());
	}
}



void MeshRenderer::Set_Shader(std::shared_ptr<Shader> shader)
{
	m_shader = shader;
	if (m_shader != nullptr) {
		has_shader = true;
		m_shader->Register_Renderer(std::dynamic_pointer_cast<MeshRenderer>(shared_from_this()));
	}
}

void MeshRenderer::Set_Material(std::shared_ptr<Material> material)
{
	if (!material->Has_Shader() || !material->Get_Shader().Initialized()) {
		Logger::LogError(LOG_POS("Set_Material"), "Attempt to set material with invalid shader.");
		return;
	}

	m_source_material = material;
	if (m_source_material != nullptr) {
		has_material = true;

		m_source_material->Get_Shader().Register_Renderer(std::dynamic_pointer_cast<MeshRenderer>(shared_from_this()));

		m_bound_material = m_source_material->Copy();
		m_bound_material->Name(m_source_material->Name() + "_clone");
		m_bound_material->Set_Shader(m_source_material->Get_Shader_Ptr());
		m_bound_material->Set_World_Object(m_object);
		m_bound_material->m_source_material = m_source_material;
		m_source_material->Register_Renderer_Material(m_bound_material);
	}
}

void MeshRenderer::Set_Mesh(std::shared_ptr<Mesh> value, bool activate_mesh)
{
	if (value == nullptr) {
		return;
	}
	if (has_default_mesh) {
		//delete m_mesh;
	}
	m_mesh = value;
	if (activate_mesh)
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


