#include "Transform.h"

#include "WorldObject.h"
#include "Logger.h"


Transform::Transform(std::weak_ptr<WorldObject> obj)
{
	assert(!obj.expired());

	m_object = obj;
	scale_mat = glm::mat4(1.0f);
	trans_mat = glm::mat4(1.0f);
	rot_mat = glm::mat4(1.0f);
	set_model_mat();
}

Transform::~Transform()
{
	Destroy();
}

void Transform::Translate(glm::vec3 value)
{
	assert(!m_object.expired());

	if (m_verbos) {
		Logger::LogDebug(LOG_POS("Translate"), "Translate transform for '%s': (%f, %f, %f)", 
			m_object.lock()->Name().c_str(), value.x, value.y, value.z);
	}
	trans_mat = glm::translate(trans_mat, value);
	set_model_mat();
	//m_position = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void Transform::Translate(float x, float y, float z)
{
	assert(!m_object.expired());
	Translate(glm::vec3(x, y, z));
}

void Transform::Rotate(glm::vec3 value)
{
	assert(!m_object.expired());
	Rotate(value.x, value.y, value.z);
}

void Transform::Rotate(float x, float y, float z)
{
	assert(!m_object.expired());

	if (m_verbos) {
		Logger::LogDebug(LOG_POS("Rotate"), "Rotate transform for '%s': (%f, %f, %f)",
			m_object.lock()->Name().c_str(), x, y, z);
	}

	m_rotation = m_rotation *
		glm::angleAxis(glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::angleAxis(glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::angleAxis(glm::radians(z), glm::vec3(0.0f, 0.0f, 1.0f));
	//rot_mat = glm::rotate(rot_mat, glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f));
	//rot_mat = glm::rotate(rot_mat, glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f));
	//rot_mat = glm::rotate(rot_mat, glm::radians(z), glm::vec3(0.0f, 0.0f, 1.0f));
	set_model_mat();
	//m_position = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	//m_rotation = model
}

void Transform::Rotation(glm::vec3 value)
{
	assert(!m_object.expired());
	Rotation(value.x, value.y, value.z);
}

void Transform::Rotation(float x, float y, float z)
{
	assert(!m_object.expired());

	if (m_verbos) {
		Logger::LogDebug(LOG_POS("Rotate"), "Rotate transform for '%s': (%f, %f, %f)",
			m_object.lock()->Name().c_str(), x, y, z);
	}

	m_rotation = 
		glm::angleAxis(glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::angleAxis(glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::angleAxis(glm::radians(z), glm::vec3(0.0f, 0.0f, 1.0f));
	//rot_mat = glm::rotate(rot_mat, glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f));
	//rot_mat = glm::rotate(rot_mat, glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f));
	//rot_mat = glm::rotate(rot_mat, glm::radians(z), glm::vec3(0.0f, 0.0f, 1.0f));
	set_model_mat();
}

void Transform::Scale(glm::vec3 value)
{
	assert(!m_object.expired());

	if (m_verbos) {
		Logger::LogDebug(LOG_POS("Scale"), "Scale transform for '%s': (%f, %f, %f)",
			m_object.lock()->Name().c_str(), value.x, value.y, value.z);
	}

	scale_mat = glm::scale(scale_mat, value);
	set_model_mat();
	//m_position = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void Transform::Scale(float x, float y, float z)
{
	assert(!m_object.expired());
	Scale(glm::vec3(x, y, z));
}

glm::vec3 Transform::Local_To_World_Point(glm::vec3 value)
{
	assert(!m_object.expired());
	return model * glm::vec4(value, 1.0f);
}

glm::vec3 Transform::Local_To_World_Direction(glm::vec3 value)
{
	assert(!m_object.expired());
	return glm::normalize(model * glm::vec4(value, 0.0f));
}

void Transform::LookAt(glm::vec3 point)
{
	assert(!m_object.expired());
	if (m_verbos) {
		Logger::LogDebug(LOG_POS("LookAt"), "Transform LookAt for '%s': (%f, %f, %f)",
			m_object.lock()->Name().c_str(), point.x, point.y, point.z);
	}

	m_rotation = glm::quatLookAt(-glm::normalize(m_position - point), glm::vec3(0.0f, 1.0f, 0.0f));
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
	assert(!m_object.expired());
	//printf("pos in: %f, %f, %f \n", pos.x, pos.y, pos.z);
	trans_mat = glm::mat4(1.0f);
	trans_mat = glm::translate(trans_mat, glm::vec3(pos.x, pos.y, pos.z));
	set_model_mat();
	//m_position = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	//printf("set to: %f, %f, %f \n", m_position.x, m_position.y, m_position.z);
}

void Transform::Destroy()
{
}

void Transform::set_model_mat(bool update_parent)
{
	assert(!m_object.expired());
	WorldObject& obj = *m_object.lock().get();

	rot_mat = glm::toMat4(m_rotation);
	if (obj.Has_Parent()) {
		WorldObject& parent = *obj.Parent().lock();
		Transform& parent_trans = parent.Get_Transform();
		if (update_parent) {
			parent_trans.set_model_mat();
		}
		model = parent_trans.Get_Model_Matrix4() * trans_mat * rot_mat * scale_mat;
	}
	else {
		model = trans_mat * rot_mat * scale_mat;
	}
	
	normal_mat = glm::transpose(glm::inverse(model));
	m_position = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_forward = glm::rotate(m_rotation, glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0, 1, 0)));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_up = glm::normalize(glm::cross(m_right, m_forward));
	//m_right = m_rotation * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	//m_up = m_rotation * glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);

	for (const auto& child : obj.Children())
	{
		assert(!child.expired());
		child.lock()->Get_Transform().set_model_mat(false);
	}
}
