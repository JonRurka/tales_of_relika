#include "Light_Material.h"

Light_Material::Light_Material(std::string name)
{
	Name(name);
}

void Light_Material::Update(float dt)
{
	
}

void Light_Material::Light_Color(glm::vec4 value)
{
	m_light_color = value;
	SetVec3("diffuseLightColor", m_light_color);
}
