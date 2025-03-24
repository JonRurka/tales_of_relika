#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

class WorldObject;

class Transform
{
	friend class WorldObject;
public:

	void Translate(glm::vec3 value);

	void Translate(float x, float y, float z);

	void Rotate(glm::vec3 value);

	void Rotate(float x, float y, float z);

	void Scale(float s) { Scale(s, s, s); }

	void Scale(glm::vec3 value);

	void Scale(float x, float y, float z);

	glm::mat4 Get_Model_Matrix4() { return model; }

	glm::mat3 Get_Normal_Matrix3() { return normal_mat; }

	glm::vec3 Local_To_World_Point(glm::vec3 value);

	glm::vec3 Local_To_World_Direction(glm::vec3 value);

	glm::vec3 Position() { return m_position; }

	void Position(glm::vec3 value) { set_position(value); }

	void LookAt(glm::vec3 point);

	glm::vec3 EulerAngles() { return m_rotation; }

	glm::vec3 Forward() { return m_forward; }

	glm::vec3 Right() { return m_right; }

	glm::vec3 Up() { return m_up; }

private:

	Transform(WorldObject* obj);

	void Update(float dt);

	void set_position(glm::vec3 pos);

	glm::mat4 scale_mat = glm::mat4(1.0f);
	glm::mat4 trans_mat = glm::mat4(1.0f);
	glm::mat4 rot_mat = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat3 normal_mat = glm::mat3(1.0f);

	glm::vec3 m_position = glm::vec3(0.0f);
	glm::vec3 m_rotation = glm::vec3(0.0f);
	glm::vec3 m_forward = glm::vec3(0.0, 0.0f, -1.0f);
	glm::vec3 m_right = glm::vec3(1.0, 0.0f, 0.0f);
	glm::vec3 m_up = glm::vec3(0.0, 1.0f, 0.0f);

	WorldObject* m_object{nullptr};

	void set_model_mat(bool update_parent = true);
};

