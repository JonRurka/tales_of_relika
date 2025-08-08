#pragma once

#include "Component.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <map>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Mesh;
class Shader;
class WorldObject;
class Renderer;
class MeshRenderer;
class Texture;

class Material : public std::enable_shared_from_this<Material>
{
	friend class Renderer;
	friend class MeshRenderer;
public:

	struct Bound_Texture {
		std::string name;
		Texture* texture;
	};

	std::string Name() { return m_name; }

	void Set_Shader(std::shared_ptr<Shader> shader);

	Shader& Get_Shader() { return *m_shader; }

	std::shared_ptr<Shader> Get_Shader_Ptr() { return m_shader; }

	bool Has_Shader() { return m_has_shader; }

	void Set_World_Object(std::weak_ptr<WorldObject> object);

	std::weak_ptr<WorldObject> Get_World_Object() { return m_object; }

	void Transparent(bool value);
	bool Transparent() { return m_is_transparent; }

	bool Is_Bound();

	void Supports_Lighting(bool value);

	bool Supports_Lighting() { return m_supports_lighting; }

	void Internal_Update(float dt, bool force = false);

	virtual std::shared_ptr<Material> Copy() { return nullptr; }
	virtual void Update(float dt) {};

	// utility uniform functions
	void setBool(const std::string& name, bool value);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void SetVec2(const std::string& name, glm::vec2 value);
	void SetVec3(const std::string& name, glm::vec3 value);
	void SetVec4(const std::string& name, glm::vec4 value);
	void setMat3x3(const std::string& name, glm::mat3 value);
	void setMat4x4(const std::string& name, glm::mat4 value);

	void setTexture(const std::string& name, std::string resource_name);
	void setTexture(const std::string& name, std::shared_ptr<Texture> value, bool set_source = true);
	void RegisterTexture(std::string name);

	void Register_Renderer_Material(std::weak_ptr<Material> mat);
	void Remove_Renderer_Material(std::weak_ptr<Material> mat);

protected:
	void Name(std::string name) { m_name = name; }

private:

	template<typename T>
	struct shader_value {
		T value;
		bool sync{ false };
		bool bound_update{ false };
	};

	struct texture_value {
		std::weak_ptr<Texture> texture;
		std::string name;
		int bind_index{ 0 };
		bool do_bind{ false };

		bool sync{ false };
		bool bound_update{ false };
	};

	bool m_supports_lighting{ false };

	std::shared_ptr<Shader> m_shader{nullptr};
	bool m_has_shader{ false };

	std::weak_ptr<WorldObject> m_object;
	std::weak_ptr<Material> m_source_material;
	std::string m_name;
	bool m_is_transparent{ false };

	static uint64_t m_next_reg_id;
	uint64_t m_registered_id{ 0 };

	//std::unordered_map<Renderer*, Material*> m_registered_materials;
	std::unordered_map<uint64_t, std::weak_ptr<Material>> m_registered_materials;

	std::unordered_map<std::string, shader_value<bool>> m_bools;
	std::unordered_map<std::string, shader_value<int>> m_ints;
	std::unordered_map<std::string, shader_value<float>> m_floats;
	std::unordered_map<std::string, shader_value<glm::vec2>> m_vec2s;
	std::unordered_map<std::string, shader_value<glm::vec3>> m_vec3s;
	std::unordered_map<std::string, shader_value<glm::vec4>> m_vec4s;
	std::unordered_map<std::string, shader_value<glm::mat3>> m_mat3s;
	std::unordered_map<std::string, shader_value<glm::mat4>> m_mat4s;

	//std::unordered_map<std::string, shader_value<Bound_Texture>> m_tex;
	//std::vector<std::string> m_registered_textures;
	std::unordered_map<std::string, texture_value> m_tex;

	// utility uniform functions
	void setBool_internal(const std::string& name, bool value) const;
	void setInt_internal(const std::string& name, int value) const;
	void setFloat_internal(const std::string& name, float value) const;
	void SetVec2_internal(const std::string& name, glm::vec2 value) const;
	void SetVec3_internal(const std::string& name, glm::vec3 value) const;
	void SetVec4_internal(const std::string& name, glm::vec4 value) const;
	void setMat3x3_internal(const std::string& name, glm::mat3 value) const;
	void setMat4x4_internal(const std::string& name, glm::mat4 value) const;

	const std::string LOG_LOC{ "MATERIAL" };

};

