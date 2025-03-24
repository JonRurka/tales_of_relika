#pragma once

#include <iostream>

#include "opengl.h"
#include "Renderer.h";

class Mesh;
class Shader;
class WorldObject;
class Material;
class Graphics;

class MeshRenderer : public Renderer
{
	friend class WorldObject;
public:

	void Set_Shader(Shader* shader);

	Shader* Get_Shader(Shader* shader) { return m_shader; }

	void Set_Material(Material* material);

	Material* Get_Material() { return m_bound_material; }

	Mesh* Get_Mesh() { return m_mesh; };

	void Set_Mesh(Mesh* value);

	void Use();

	void Draw(float dt) override;

private:
	MeshRenderer(WorldObject* obj);

	void Update(float dt);

	WorldObject* m_object{ nullptr };
	Mesh* m_mesh{ nullptr };
	Shader* m_shader{ nullptr };
	Material* m_source_material{ nullptr };
	Material* m_bound_material{ nullptr };

	bool has_default_mesh = true;
	bool has_shader = false;
	bool has_material = false;

	void update_model_matrix();

};

