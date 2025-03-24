#include "Transform.h"

#include "WorldObject.h"


Transform::Transform(WorldObject* obj)
{
	m_object = obj;
	scale_mat = glm::mat4(1.0f);
	trans_mat = glm::mat4(1.0f);
	rot_mat = glm::mat4(1.0f);
	set_model_mat();
}

void Transform::Translate(glm::vec3 value)
{
	trans_mat = glm::translate(trans_mat, value);
	set_model_mat();
	//m_position = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void Transform::Translate(float x, float y, float z)
{
	Translate(glm::vec3(x, y, z));
}

void Transform::Rotate(glm::vec3 value)
{
	Rotate(value.x, value.y, value.z);
}

void Transform::Rotate(float x, float y, float z)
{
	rot_mat = glm::rotate(rot_mat, glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f));
	rot_mat = glm::rotate(rot_mat, glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f));
	rot_mat = glm::rotate(rot_mat, glm::radians(z), glm::vec3(0.0f, 0.0f, 1.0f));
	set_model_mat();
	//m_position = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	//m_rotation = model
}

void Transform::Scale(glm::vec3 value)
{
	scale_mat = glm::scale(scale_mat, value);
	set_model_mat();
	//m_position = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void Transform::Scale(float x, float y, float z)
{
	Scale(glm::vec3(x, y, z));
}

glm::vec3 Transform::Local_To_World_Point(glm::vec3 value)
{
	return model * glm::vec4(value, 1.0f);
}

glm::vec3 Transform::Local_To_World_Direction(glm::vec3 value)
{
	return glm::normalize(model * glm::vec4(value, 0.0f));
}

void Transform::LookAt(glm::vec3 point)
{

	glm::quat q = glm::quatLookAt(-glm::normalize(m_position - point), glm::vec3(0.0f, 1.0f, 0.0f));
	rot_mat = glm::toMat4(q);
	set_model_mat();

	/*if (glm::length(point) <= 0.00001f) {
		point.x = 0.0001f;
	}
	printf("######\n");
	printf("mat: %f, %f, %f\n", rot_mat[0][1], rot_mat[0][2], rot_mat[0][3]);
	printf("pos: %f, %f, %f\n", m_position.x, m_position.y, m_position.z);
	printf("point: %f, %f, %f\n", point.x, point.y, point.z);
	rot_mat = glm::lookAt(m_position, point, glm::vec3(0.0f, 1.0f, 0.0f));
	//printf("%f, %f, %f\n", rot_mat[0][1], rot_mat[0][2], rot_mat[0][3]);
	set_model_mat();
	printf("final pos: %f, %f, %f\n", m_position.z, m_position.y, m_position.z);*/
}

void Transform::Update(float dt)
{
}

void Transform::set_position(glm::vec3 pos)
{
	//printf("pos in: %f, %f, %f \n", pos.x, pos.y, pos.z);
	trans_mat = glm::mat4(1.0f);
	trans_mat = glm::translate(trans_mat, glm::vec3(pos.x, pos.y, pos.z));
	set_model_mat();
	//m_position = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	//printf("set to: %f, %f, %f \n", m_position.x, m_position.y, m_position.z);
}

void Transform::set_model_mat(bool update_parent)
{
	WorldObject* parent = m_object->Parent();
	if (parent != nullptr) {
		Transform* parent_trans = parent->Get_Transform();
		if (update_parent) {
			parent_trans->set_model_mat();
		}
		model = parent_trans->Get_Model_Matrix4() * trans_mat * rot_mat * scale_mat;
	}
	else {
		model = trans_mat * rot_mat * scale_mat;
	}
	
	normal_mat = glm::transpose(glm::inverse(model));
	m_position = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_forward = model * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	m_right = model * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	m_up = model * glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);

	for (const auto& child : m_object->Children())
	{
		child->Get_Transform()->set_model_mat(false);
	}
}
