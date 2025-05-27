#pragma once

#include "opengl.h"
#include "Component.h"

#include "dynamic_compute.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class WorldObject;
class Transform;
class Framebuffer;
class Texture;
class Cubemap;
class Mesh;
class Shader;
class GPUSort;

class Camera : public Component
{
	friend class WorldObject;
public:

	float FOV() { return m_FOV; }
	void FOV(float value) 
	{ 
		m_FOV = value;
		update_projection_matrix();
	}

	float Near_Plane() { return m_near; }
	void Near_Plane(float value) 
	{ 
		m_near = value;
		update_projection_matrix();
	}

	float Far_Plane() { return m_far; }
	void Far_Plane(float value) 
	{ 
		m_far = value;
		update_projection_matrix();
	}

	glm::vec4 Clear_Color() { return m_clear_color; }
	void Clear_Color(glm::vec4 value) { m_clear_color = value; }

	void Set_Skybox(Cubemap* value);

	void ScreenPointToRay(glm::vec2 pos, glm::vec3& out_start, glm::vec3& out_dir);

	Texture* FrameTexture();

	glm::mat4 Projection_Matrix() { return m_projection; }
	glm::mat4 View_Matrix() { return m_view; }

	static Camera* Get_Active();
	
	void Activate(bool active);

private:
	glm::mat4 m_projection;
	glm::mat4 m_view;

	//glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	unsigned int m_screen_width = 800;
	unsigned int m_screen_height = 600;
	float m_FOV = 45.0f;
	float m_near = 0.1f;
	float m_far = 1000.0f;
	bool m_isActive{ false };
	glm::vec4 m_clear_color{glm::ivec4(0.0f, 0.0f, 0.0f, 1.0f)};

	Cubemap* m_cubemap{nullptr};
	Mesh* m_cubemap_mesh{ nullptr };
	Shader* m_cubemap_shader{ nullptr };

	Framebuffer* m_framebuffer{nullptr};

	GPUSort* m_sort{ nullptr };


	unsigned int Screen_Width() { return m_screen_width; }
	unsigned int Screen_Height() { return m_screen_height; }

	Transform* m_transform;

	static Camera* m_active_camera;

	void create_framebuffer();

	void update_view_matrix();
	void update_projection_matrix();

	void render_skybox(float dt);
	void render_opaque(float dt);
	void render_transparent(float dt);
	void render(float dt);


	inline static const std::string LOG_LOC{ "CAMERA" };

protected:
	void Init() override;
	void Update(float dt) override;
	void Load(json data) override;

};

