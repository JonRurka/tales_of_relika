#pragma once

#include "Material.h"

class Light_Material : public Material
{
public:

	Light_Material(std::string name);

	Material* Copy() override { return new Light_Material(Name()); };

	void Update(float dt) override;

	void Light_Color(glm::vec4 value);

	glm::vec4 Light_Color() { return m_light_color; }

private:
	glm::vec4 m_light_color{ glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) };

};

