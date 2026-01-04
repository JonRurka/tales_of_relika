#include "Camera.h"

#include "WorldObject.h"
#include "Transform.h"
#include "Framebuffer.h"
#include "Logger.h"
#include "Shader.h"
#include "Renderer.h"
#include "Graphics.h"
#include "Mesh.h"
#include "Cubemap.h"
#include "Scene.h"
#include "GPUSort.h"
#include "Resources.h"
#include "Texture.h"

#include "tracy/Tracy.hpp"

#include <math.h>



#define SKYBOX_VERT_SHADER "graphics::engine::skybox::skybox.vert"
#define SKYBOX_FRAG_SHADER "graphics::engine::skybox::skybox.frag"

#define SKYBOX_PROJECTION_LOC	10
#define SKYBOX_VIEW_LOC			11

std::weak_ptr<Camera> Camera::m_active_camera;
std::shared_ptr<Shader> Camera::m_cubemap_shader{ nullptr };
std::shared_ptr<Mesh> Camera::m_cubemap_mesh{ nullptr };


namespace {
	glm::vec3 getRayFromScreenSpace(const glm::vec2& pos, const float halfScreenWidth, const float halfScreenHeight, const glm::mat4 perspective, const glm::mat4 view)
	{
		glm::mat4 invMat = glm::inverse(perspective * view);
		glm::vec4 near = glm::vec4((pos.x - halfScreenWidth) / halfScreenWidth, -1 * (pos.y - halfScreenHeight) / halfScreenHeight, -1, 1.0);
		glm::vec4 far = glm::vec4((pos.x - halfScreenWidth) / halfScreenWidth, -1 * (pos.y - halfScreenHeight) / halfScreenHeight, 1, 1.0);
		glm::vec4 nearResult = invMat * near;
		glm::vec4 farResult = invMat * far;
		nearResult /= nearResult.w;
		farResult /= farResult.w;
		glm::vec3 dir = glm::vec3(farResult - nearResult);
		return glm::normalize(dir);
	}
}

void Camera::Init()
{
	Type_Name("Camera");
	//m_transform = Object().Get_Transform();
	update_projection_matrix();
	update_view_matrix();
	create_framebuffer();
	if (m_active_camera.expired()) {
		Logger::LogDebug(LOG_POS("Init"), "Setting %s as first active camera.", Object().Name().c_str());
		Activate(true);
	}
	//m_sort = new GPUSort(1024);
}

void Camera::Update(float dt)
{
	update_view_matrix();
	//render(dt);
}

void Camera::Load(json data)
{
	data["fov"].get_to(m_FOV);
	data["near"].get_to(m_near);
	data["far"].get_to(m_far);
	data["active"].get_to(m_isActive);
	update_projection_matrix();
}

void Camera::OnDestroy()
{
	Logger::LogDebug(LOG_POS("OnDestroy"), "Camera destroyed.");

	Activate(false);

	destroy_framebuffer();
	destroy_skybox();
}

void Camera::Set_Skybox(std::shared_ptr<Cubemap> value)
{
	if (m_cubemap_mesh == nullptr) 
	{
		std::vector<glm::vec4> skyboxVertices = {
			// positions          
			glm::vec4(-1.0f,  1.0f, -1.0f, 0.0f),
			glm::vec4(-1.0f, -1.0f, -1.0f, 0.0f),
			glm::vec4( 1.0f, -1.0f, -1.0f, 0.0f),
			glm::vec4( 1.0f, -1.0f, -1.0f, 0.0f),
			glm::vec4( 1.0f,  1.0f, -1.0f, 0.0f),
			glm::vec4(-1.0f,  1.0f, -1.0f, 0.0f),
										 
			glm::vec4(-1.0f, -1.0f,  1.0f, 0.0f),
			glm::vec4(-1.0f, -1.0f, -1.0f, 0.0f),
			glm::vec4(-1.0f,  1.0f, -1.0f, 0.0f),
			glm::vec4(-1.0f,  1.0f, -1.0f, 0.0f),
			glm::vec4(-1.0f,  1.0f,  1.0f, 0.0f),
			glm::vec4(-1.0f, -1.0f,  1.0f, 0.0f),
										 
			glm::vec4( 1.0f, -1.0f, -1.0f, 0.0f),
			glm::vec4( 1.0f, -1.0f,  1.0f, 0.0f),
			glm::vec4( 1.0f,  1.0f,  1.0f, 0.0f),
			glm::vec4( 1.0f,  1.0f,  1.0f, 0.0f),
			glm::vec4( 1.0f,  1.0f, -1.0f, 0.0f),
			glm::vec4( 1.0f, -1.0f, -1.0f, 0.0f),
										 
			glm::vec4(-1.0f, -1.0f,  1.0f, 0.0f),
			glm::vec4(-1.0f,  1.0f,  1.0f, 0.0f),
			glm::vec4( 1.0f,  1.0f,  1.0f, 0.0f),
			glm::vec4( 1.0f,  1.0f,  1.0f, 0.0f),
			glm::vec4( 1.0f, -1.0f,  1.0f, 0.0f),
			glm::vec4(-1.0f, -1.0f,  1.0f, 0.0f),
										 
			glm::vec4(-1.0f,  1.0f, -1.0f, 0.0f),
			glm::vec4( 1.0f,  1.0f, -1.0f, 0.0f),
			glm::vec4( 1.0f,  1.0f,  1.0f, 0.0f),
			glm::vec4( 1.0f,  1.0f,  1.0f, 0.0f),
			glm::vec4(-1.0f,  1.0f,  1.0f, 0.0f),
			glm::vec4(-1.0f,  1.0f, -1.0f, 0.0f),
										 
			glm::vec4(-1.0f, -1.0f, -1.0f, 0.0f),
			glm::vec4(-1.0f, -1.0f,  1.0f, 0.0f),
			glm::vec4( 1.0f, -1.0f, -1.0f, 0.0f),
			glm::vec4( 1.0f, -1.0f, -1.0f, 0.0f),
			glm::vec4(-1.0f, -1.0f,  1.0f, 0.0f),
			glm::vec4( 1.0f, -1.0f,  1.0f, 0.0f)
		};
		m_cubemap_mesh = std::make_shared<Mesh>();
		m_cubemap_mesh->Vertices(skyboxVertices);
		m_cubemap_mesh->Activate();
	}
	if (m_cubemap_shader == nullptr)
	{
		m_cubemap_shader = Shader::Create("skybox", SKYBOX_VERT_SHADER, SKYBOX_FRAG_SHADER);
		if (m_cubemap_shader == nullptr || !m_cubemap_shader->Initialized()) {
			Logger::LogError(LOG_POS("Set_Skybox"), "Failed to create skybox shader.");
			return;
		}
	}
	m_cubemap = value;
	m_has_skybox = true;
	//Logger::LogDebug(LOG_POS("Set_Skybox"), "Skybox set successfully.");
}

void Camera::ScreenPointToRay(glm::vec2 screen_pos, glm::vec3& start, glm::vec3& dir)
{


	float width = Graphics::Width();
	float height = Graphics::Height();

	float x = (2.0f * ((float)(screen_pos.x - 0) / (width - 0))) - 1.0f;
	float y = 1.0f - (2.0f * ((float)(screen_pos.y - 0) / (height - 0)));

	glm::vec4 point(x, y, 0.0f, 1.0f);

	glm::mat4 pv = m_projection * m_view;
	glm::mat4 inv_pv = glm::inverse(pv);

	dir = getRayFromScreenSpace(screen_pos, width / 2, height / 2, m_projection, m_view);

	glm::vec4 origin;
	origin = inv_pv * point;
	origin /= origin.w;
	start = origin;

	bool start_null = glm::all(glm::isnan(start));
	bool dir_null = glm::all(glm::isnan(dir));
	if (start_null || dir_null) {
		start = glm::vec3(0);
		dir = glm::vec3(0, 0, 1);
		Logger::LogError(LOG_POS("ScreenPointToRay"), "Resultant Start or Dir null!");
	}
}

Texture& Camera::FrameTexture()
{
	return *m_cam_framebuffer->Active_Texture();
}

Camera& Camera::Get_Active()
{
	assert(Has_Active_Camera());
	return *m_active_camera.lock();
}

bool Camera::Has_Active_Camera()
{
	return !m_active_camera.expired();
}

void Camera::Activate(bool active)
{
	if ((m_isActive && active) ||
		(!m_isActive && !active))
		return; // No change.

	if (active && !m_isActive) {
		m_isActive = true;
		if (!m_active_camera.expired()) {
			m_active_camera.lock()->Activate(false);
		}
		m_active_camera = std::dynamic_pointer_cast<Camera>(shared_from_this());
		m_active_camera.lock()->Resize_Refresh();
		Graphics::Instance().Set_Screen_FrameTexture(m_cam_framebuffer->Active_Texture());
		Logger::LogDebug(LOG_POS("Activate"), "Set new active camera.");
	}
	else if (!active && m_isActive) {
		m_isActive = false;
		m_active_camera.reset();
		Graphics::Instance().Set_Screen_FrameTexture(nullptr);
		Logger::LogDebug(LOG_POS("Activate"), "Camera set inactive.");
	}
}

void Camera::Resize_Refresh()
{
	update_projection_matrix();
}

void Camera::create_framebuffer()
{
	m_cam_framebuffer = std::make_shared<Framebuffer>();
	m_cam_framebuffer->Init();
	if (!m_cam_framebuffer->Complete())
	{
		Logger::LogError(LOG_POS("create_framebuffer"), "Failed to create framebuffer.");
	}
}

void Camera::destroy_framebuffer()
{
	if (m_cam_framebuffer.get() != nullptr)
	{
		m_cam_framebuffer->Dispose();
	}
}

void Camera::destroy_skybox()
{
	if (!m_has_skybox)
		return;

	m_cubemap->Dispose();
	m_has_skybox = false;
}

void Camera::update_view_matrix()
{
	glm::vec3 front_global = Object().Get_Transform().Local_To_World_Direction(cameraFront);
	glm::vec3 cam_pos = Object().Get_Transform().Position();
	m_view = glm::lookAt(cam_pos, cam_pos + front_global, cameraUp);
	//printf("update view matrix: %f, %f, %f\n", cam_pos.x, cam_pos.y, cam_pos.z);
}

void Camera::update_projection_matrix()
{
	m_projection = glm::perspective(glm::radians(m_FOV), Graphics::Width() / (float)Graphics::Height(), m_near, m_far);
}

void Camera::render_skybox(float dt)
{
	if (!m_has_skybox || 
		m_cubemap == nullptr || 
		m_cubemap_mesh == nullptr || 
		m_cubemap_shader == nullptr ||
		!m_cubemap_shader->Initialized()) {
		return;
	}

	ZoneScopedN("camera render skybox");

	glDepthFunc(GL_LEQUAL);
	m_cubemap_shader->use(false);

	//m_cubemap->Bind();

	//m_cubemap_shader->setInt("skybox", 0);
	glm::mat4 view = glm::mat4(glm::mat3(m_view));
	m_cubemap_shader->setMat4x4("projection", m_projection);
	m_cubemap_shader->setMat4x4("view", view);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap->Tex());
	//glBindTexture(GL_TEXTURE_2D, Resources::Get_Texture("skybox::right.jpg")->Tex());

	m_cubemap_mesh->Draw();

	glDepthFunc(GL_LESS);
}

void Camera::render_opaque(float dt)
{
	ZoneScopedN("camera render opaque");

	m_alpha_renderers.clear();
	m_alpha_renderers.reserve(Object().scene().Objects().size());

	m_alpha_object_idx.clear();
	m_alpha_object_idx.reserve(Object().scene().Objects().size());
	
	glm::vec3 cam_pos = Object().Get_Transform().Position();

	int i = 0;
	std::vector<uint64_t> shader_ids = Shader::Get_Shader_ID_List();
	for (const auto& ID : shader_ids) {
		Shader& shader = *Shader::Get_Shader(ID);
		std::vector<std::weak_ptr<Renderer>> renderers = Shader::Get_Shader_Renderer_List(ID);
		if (renderers.size() <= 0)
			continue;
		shader.use(true);
		for (const auto& rend : renderers) {
			assert(!rend.expired());
			Renderer& renderer = *rend.lock();
			if (renderer.Transparent()) {
				m_alpha_renderers.push_back(rend);
				float dist = glm::distance(cam_pos, renderer.worldObject().lock()->Get_Transform().Position());
				glm::vec4 a_map = glm::vec4((float)i++, dist, 0.0f, 0.0f);
				m_alpha_object_idx.push_back(a_map);
				continue;
			}
			renderer.Draw(dt);
		}

	}
}

void Camera::render_transparent(float dt)
{
	ZoneScopedN("camera render transparent");

	for (const auto& elem : m_alpha_object_idx) {
		int i = lround(elem.x);
		assert(!m_alpha_renderers[i].expired());
		Renderer& rend = *m_alpha_renderers[i].lock();
		rend.Draw(dt);
	}
}

void Camera::render(float dt)
{
	ZoneScopedN("camera render");

	assert(m_cam_framebuffer.get() != nullptr);
	m_cam_framebuffer->Use(true);

	glClearColor(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a); // Yellow
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	render_opaque(dt);
	//render_transparent(dt);
	render_skybox(dt);

	m_cam_framebuffer->Use(false);
}

void Camera::StaticDestroy()
{

	if (m_cubemap_mesh != nullptr)
	{
		m_cubemap_mesh->Dispose();
		m_cubemap_mesh.reset();
	}

	if (m_cubemap_shader != nullptr)
	{
		m_cubemap_shader->Dispose();
		m_cubemap_shader.reset();
		//Shader::Remove(m_cubemap_shader);
	}

}
