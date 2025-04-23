#include "Material.h"

#include "Shader.h"
#include "Logger.h"
#include "Texture.h"
#include "Resources.h"

#define Update_Uniforms(func, u_map, force)			\
do {												\
	for (const auto& pair : u_map) {				\
		if (pair.second.sync ||						\
			pair.second.bound_update || force) {	\
			func(pair.first, pair.second.value);	\
			u_map[pair.first].sync = false;			\
		}											\
	}												\
} while (false)

#define Mat_Set(func, name, val)						\
do {													\
	for (const auto& pair : m_registered_materials) {	\
		pair.second->func(name, value);					\
	}													\
} while (false)

void Material::Set_Shader(Shader* shader)
{
	if (shader == nullptr ||
		!shader->Initialized()) {
		Logger::LogError(LOG_POS("Set_Shader"), "Material '%s' set to invalid shader.", Name().c_str());
		return;
	}

	m_shader = shader;
	m_shader->Register_Material(this);
}

void Material::Set_World_Object(WorldObject* object)
{
	m_object = object;
}

void Material::Transparent(bool value)
{ 
	m_is_transparent = value;
	if (!Is_Bound()) {
		for (const auto& pair : m_registered_materials)
		{
			pair.second->Transparent(value);
		}
	}
}


void Material::setBool(const std::string& name, bool value)
{
	m_bools[name] = shader_value<bool>{ value, true, Is_Bound()};
	Mat_Set(setBool, name, value);
}

void Material::setInt(const std::string& name, int value)
{
	m_ints[name] = shader_value<int>{ value, true, Is_Bound() };
	Mat_Set(setInt, name, value);
}

void Material::setFloat(const std::string& name, float value)
{
	m_floats[name] = shader_value<float>{ value, true, Is_Bound() };
	Mat_Set(setFloat, name, value);
}

void Material::SetVec2(const std::string& name, glm::vec2 value)
{
	m_vec2s[name] = shader_value<glm::vec2>{ value, true, Is_Bound() };
	Mat_Set(SetVec2, name, value);
}

void Material::SetVec3(const std::string& name, glm::vec3 value)
{
	m_vec3s[name] = shader_value<glm::vec3>{ value, true, Is_Bound() };
	//printf("%s (bound: %i) Material::SetVec3: set %s: %i \n", m_name.c_str(), (int)Is_Bound(), name.c_str(), (int)m_vec3s.size());
	Mat_Set(SetVec3, name, value);
}

void Material::SetVec4(const std::string& name, glm::vec4 value)
{
	m_vec4s[name] = shader_value<glm::vec4>{ value, true, Is_Bound() };
	Mat_Set(SetVec4, name, value);
}

void Material::setMat3x3(const std::string& name, glm::mat3 value)
{
	m_mat3s[name] = shader_value<glm::mat3>{ value, true, Is_Bound() };
	Mat_Set(setMat3x3, name, value);
}

void Material::setMat4x4(const std::string& name, glm::mat4 value)
{
	m_mat4s[name] = shader_value<glm::mat4>{ value, true, Is_Bound() };
	Mat_Set(setMat4x4, name, value);
}

void Material::setTexture(const std::string& name, std::string resource_name)
{
	if (!Resources::Has_Texture(resource_name)) {
		Logger::LogError(LOG_POS("setTexture"), "Texture resource %s does not exist.", resource_name.c_str());
		return;
	}
	Texture* tex = Resources::Get_Texture(resource_name);
	setTexture(name, tex);
}

void Material::setTexture(const std::string& name, Texture* value, bool set_source)
{
	if (m_tex.find(name) == m_tex.end())
	{
		Logger::LogError(LOG_POS("setTexture"), "Texture Sampler %s not bound to material %s", name.c_str(), Name().c_str());
		return;
	}

	if (Is_Bound() && set_source) {
		m_source_material->setTexture(name, value);
	//	m_tex[name] = m_source_material->m_tex[name];
	}

	m_tex[name].do_bind = true;
	m_tex[name].texture = value;

	for (const auto& pair : m_registered_materials) {
		pair.second->setTexture(name, value, false);
	}
	
	//m_tex[name].sync = true;
	//m_tex[name].bound_update = Is_Bound();


	//Mat_Set(setTexture, name, value);

	/*m_tex[value.name] = shader_value<Bound_Texture>{value, true, Is_Bound()};
	Mat_Set(setTexture, value.name, value);
	for (const auto& pair : m_registered_materials) {
			pair.second->setTexture(value);
	}*/
}

void Material::RegisterTexture(std::string name)
{
	if (Is_Bound())
	{
		m_source_material->RegisterTexture(name);
		return;
	}
	texture_value val;
	val.name = name;
	val.bind_index = m_tex.size();

	val.texture = nullptr;
	val.do_bind = false;

	//val.bound_update = false;
	//val.sync = true;

	m_tex[name] = val;

	/*if (Is_Bound())
		m_source_material->m_registered_textures.push_back(name);
	else
		m_registered_textures.push_back(name);*/
}



void Material::Register_Renderer_Material(Renderer* rend, Material* mat)
{
	m_registered_materials[rend] = mat;
}

void Material::Supports_Lighting(bool value)
{
	m_supports_lighting = true;
}

void Material::Internal_Update(float dt, bool force)
{
	Update_Uniforms(setBool_internal, m_bools, force);
	Update_Uniforms(setInt_internal, m_ints, force);
	Update_Uniforms(setFloat_internal, m_floats, force);
	Update_Uniforms(SetVec2_internal, m_vec2s, force);
	Update_Uniforms(SetVec3_internal, m_vec3s, force);
	Update_Uniforms(SetVec4_internal, m_vec4s, force);
	Update_Uniforms(setMat3x3_internal, m_mat3s, force);
	Update_Uniforms(setMat4x4_internal, m_mat4s, force);

	// Bind textures
	for (const auto& pair : m_tex) {
		if (pair.second.do_bind || force) {
			m_shader->setInt(pair.second.name, pair.second.bind_index);
			pair.second.texture->Bind(GL_TEXTURE0 + pair.second.bind_index);
		}
	}

	Update(dt);
}


void Material::setBool_internal(const std::string& name, bool value) const
{
	if (m_shader == nullptr)
		return;
	m_shader->setBool(name, value);
}

void Material::setInt_internal(const std::string& name, int value) const
{
	if (m_shader == nullptr)
		return;
	m_shader->setInt(name, value);
}

void Material::setFloat_internal(const std::string& name, float value) const
{
	if (m_shader == nullptr)
		return;
	m_shader->setFloat(name, value);
}

void Material::SetVec2_internal(const std::string& name, glm::vec2 value) const
{
	if (m_shader == nullptr)
		return;
	m_shader->SetVec2(name, value);
}

void Material::SetVec3_internal(const std::string& name, glm::vec3 value) const
{
	if (m_shader == nullptr)
		return;
	m_shader->SetVec3(name, value);
}

void Material::SetVec4_internal(const std::string& name, glm::vec4 value) const
{
	if (m_shader == nullptr)
		return;
	m_shader->SetVec4(name, value);
}

void Material::setMat3x3_internal(const std::string& name, glm::mat3 value) const
{
	if (m_shader == nullptr)
		return;
	m_shader->setMat3x3(name, value);
}

void Material::setMat4x4_internal(const std::string& name, glm::mat4 value) const
{
	if (m_shader == nullptr)
		return;
	m_shader->setMat4x4(name, value);
}


