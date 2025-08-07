#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "opengl.h"
#include "Renderer.h"

#include "Shader.h"

class Mesh;
class Shader;
class WorldObject;
class Material;
class Graphics;

class MeshRenderer : public Renderer
{
	friend class WorldObject;
public:

	~MeshRenderer();

	void Set_Shader(std::shared_ptr<Shader> shader);

	Shader& Get_Shader() { return *m_shader; }

	void Set_Material(std::shared_ptr<Material> material);

	Material& Get_Material() { return *m_bound_material; }

	Mesh& Get_Mesh() { return *m_mesh; };

	void Set_Mesh(std::shared_ptr<Mesh> value, bool activate_mesh = true);

	void Use();

	void Draw(float dt) override;

private:
	MeshRenderer(std::weak_ptr<WorldObject> obj);

	void Update(float dt);

	void OnDestroy() override;

	//void Destroy();

	std::weak_ptr<WorldObject> m_object;
	std::shared_ptr<Mesh> m_mesh{ nullptr };
	std::shared_ptr<Shader> m_shader{ nullptr };
	std::shared_ptr<Material> m_source_material{ nullptr };
	std::shared_ptr<Material> m_bound_material{ nullptr };

	bool has_default_mesh = true;
	bool has_shader = false;
	bool has_material = false;

	void update_model_matrix();

	inline static const std::string LOG_LOC{ "MESH_RENDERER" };

};

