#pragma once

#include "opengl.h"
#include "Component.h"

#include "dynamic_compute.h"

#include "Frustum.h"

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Engine;
class Graphics;
class WorldObject;
class Transform;
class Framebuffer;
class Texture;
class Renderer;
class Cubemap;
class Mesh;
class Shader;
class GPUSort;

using namespace BoundsVolume;

class Camera : public Component
{
	friend class WorldObject;
	friend class Engine;
	friend class Graphics;
	
public:
	typedef std::shared_ptr<Camera> Shared;
	typedef std::weak_ptr<Camera> Weak;

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

	static float Aspect();

	glm::vec4 Clear_Color() { return m_clear_color; }
	void Clear_Color(glm::vec4 value) { m_clear_color = value; }

	void Set_Skybox(std::shared_ptr<Cubemap> value);

	void ScreenPointToRay(glm::vec2 pos, glm::vec3& out_start, glm::vec3& out_dir);

	Texture& FrameTexture();

	glm::mat4 Projection_Matrix() { return m_projection; }
	glm::mat4 View_Matrix() { return m_view; }

	static Camera& Get_Active();
	static Camera::Weak Get_Active_Ptr() { return m_active_camera; }

	static bool Has_Active_Camera();
	
	void Activate(bool active);

	void Resize_Refresh();

private:
	glm::mat4 m_projection;
	glm::mat4 m_view;

	//glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float m_FOV = 45.0f;
	float m_near = 0.1f;
	float m_far = 1000.0f;
	bool m_isActive{ false };
	glm::vec4 m_clear_color{glm::ivec4(0.0f, 0.0f, 0.0f, 1.0f)};

	std::shared_ptr<Cubemap> m_cubemap{nullptr};
	static std::shared_ptr<Mesh> m_cubemap_mesh;
	static std::shared_ptr<Shader> m_cubemap_shader;
	bool m_has_skybox{ false };

	std::shared_ptr<Framebuffer> m_cam_framebuffer;

	std::vector<std::weak_ptr<Renderer>> m_alpha_renderers;
	std::vector<glm::vec4> m_alpha_object_idx;

	GPUSort* m_sort{ nullptr };

	Frustum m_frustum;

	//Transform* m_transform{ nullptr };

	static std::weak_ptr<Camera> m_active_camera;

	void create_framebuffer();
	void destroy_framebuffer();

	void destroy_skybox();

	void update_view_matrix();
	void update_projection_matrix();

	void render_skybox(float dt);
	void render_opaque(float dt);
	void render_transparent(float dt);
	void render(float dt);

	void refresh_frustum();
	Frustum createFrustumFromCamera();

	// Called by the engine on shutdown.
	static void StaticDestroy();


	inline static const std::string LOG_LOC{ "CAMERA" };

protected:
	void Init() override;
	void Update(float dt) override;
	void Load(json data) override;
	void OnDestroy() override;
};

