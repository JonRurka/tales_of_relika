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

#include <math.h>



#define SKYBOX_VERT_SHADER "graphics.engine.skybox.skybox.vert"
#define SKYBOX_FRAG_SHADER "graphics.engine.skybox.skybox.frag"

Camera* Camera::m_active_camera{nullptr};


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
	m_transform = Object()->Get_Transform();
	update_projection_matrix();
	update_view_matrix();
	create_framebuffer();
	if (Get_Active() == nullptr) {
		Activate(true);
	}
	m_sort = new GPUSort(1024);
}

void Camera::Update(float dt)
{
	update_view_matrix();
	render(dt);
}

void Camera::Load(json data)
{
	data["fov"].get_to(m_FOV);
	data["near"].get_to(m_near);
	data["far"].get_to(m_far);
	data["active"].get_to(m_isActive);
	update_projection_matrix();
}

void Camera::Set_Skybox(Cubemap* value)
{
	if (m_cubemap_mesh == nullptr) {
		std::vector<glm::vec3> skyboxVertices = {
			// positions          
			glm::vec3(-1.0f,  1.0f, -1.0f),
			glm::vec3(-1.0f, -1.0f, -1.0f),
			glm::vec3( 1.0f, -1.0f, -1.0f),
			glm::vec3( 1.0f, -1.0f, -1.0f),
			glm::vec3( 1.0f,  1.0f, -1.0f),
			glm::vec3(-1.0f,  1.0f, -1.0f),
										 
			glm::vec3(-1.0f, -1.0f,  1.0f),
			glm::vec3(-1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f,  1.0f, -1.0f),
			glm::vec3(-1.0f,  1.0f, -1.0f),
			glm::vec3(-1.0f,  1.0f,  1.0f),
			glm::vec3(-1.0f, -1.0f,  1.0f),
										 
			glm::vec3( 1.0f, -1.0f, -1.0f),
			glm::vec3( 1.0f, -1.0f,  1.0f),
			glm::vec3( 1.0f,  1.0f,  1.0f),
			glm::vec3( 1.0f,  1.0f,  1.0f),
			glm::vec3( 1.0f,  1.0f, -1.0f),
			glm::vec3( 1.0f, -1.0f, -1.0f),
										 
			glm::vec3(-1.0f, -1.0f,  1.0f),
			glm::vec3(-1.0f,  1.0f,  1.0f),
			glm::vec3( 1.0f,  1.0f,  1.0f),
			glm::vec3( 1.0f,  1.0f,  1.0f),
			glm::vec3( 1.0f, -1.0f,  1.0f),
			glm::vec3(-1.0f, -1.0f,  1.0f),
										 
			glm::vec3(-1.0f,  1.0f, -1.0f),
			glm::vec3( 1.0f,  1.0f, -1.0f),
			glm::vec3( 1.0f,  1.0f,  1.0f),
			glm::vec3( 1.0f,  1.0f,  1.0f),
			glm::vec3(-1.0f,  1.0f,  1.0f),
			glm::vec3(-1.0f,  1.0f, -1.0f),
										 
			glm::vec3(-1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f, -1.0f,  1.0f),
			glm::vec3( 1.0f, -1.0f, -1.0f),
			glm::vec3( 1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f, -1.0f,  1.0f),
			glm::vec3( 1.0f, -1.0f,  1.0f)
		};
		m_cubemap_mesh = new Mesh();
		m_cubemap_mesh->Activate();
		m_cubemap_mesh->Vertices(skyboxVertices);
		m_cubemap_shader = Shader::Create("skybox", SKYBOX_VERT_SHADER, SKYBOX_FRAG_SHADER);
		if (!m_cubemap_shader->Initialized()) {
			Logger::LogError(LOG_POS("Set_Skybox"), "Failed to create skybox shader.");
			return;
		}
	}
	m_cubemap = value;
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
}

Texture* Camera::FrameTexture()
{
	return m_framebuffer->Active_Texture();
}

Camera* Camera::Get_Active()
{
	return m_active_camera;
}

void Camera::Activate(bool active)
{
	if ((m_isActive && active) ||
		(!m_isActive && !active))
		return;

	if (active && !m_isActive) {
		m_isActive = true;
		if (m_active_camera) {
			m_active_camera->Activate(false);
		}
		m_active_camera = this;
		Graphics::Instance()->Set_Screen_FrameTexture(m_framebuffer->Active_Texture());
	}
	else if (!active && m_isActive) {
		m_isActive = false;
	}
}

void Camera::create_framebuffer()
{
	m_framebuffer = new Framebuffer();
	if (!m_framebuffer->Complete())
	{
		Logger::LogError(LOG_POS("create_framebuffer"), "Failed to create framebuffer.");
	}
}

void Camera::update_view_matrix()
{
	glm::vec3 front_global = m_transform->Local_To_World_Direction(cameraFront);
	glm::vec3 cam_pos = m_transform->Position();
	m_view = glm::lookAt(cam_pos, cam_pos + front_global, cameraUp);
	//printf("update view matrix: %f, %f, %f\n", cam_pos.x, cam_pos.y, cam_pos.z);
}

void Camera::update_projection_matrix()
{
	m_projection = glm::perspective(glm::radians(m_FOV), Screen_Width() / (float)Screen_Height(), m_near, m_far);
}

void Camera::render_skybox(float dt)
{
	if (m_cubemap == nullptr) {
		return;
	}

	glDepthFunc(GL_LEQUAL);
	m_cubemap_shader->use(false);

	//m_cubemap->Bind();

	m_cubemap_shader->setInt("skybox", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap->Tex());
	glm::mat4 view = glm::mat4(glm::mat3(m_view));
	m_cubemap_shader->setMat4x4("projection", m_projection);
	m_cubemap_shader->setMat4x4("view", view);

	m_cubemap_mesh->Draw();

	glDepthFunc(GL_LESS);
}

std::vector<Renderer*> alpha_renderers;
std::vector<glm::vec4> alpha_object_idx;

void Camera::render_opaque(float dt)
{
	//alpha_renderers.clear();
	//alpha_renderers.reserve(Object()->scene()->Objects().size());

	alpha_object_idx.clear();
	alpha_object_idx.reserve(Object()->scene()->Objects().size());
	
	int i = 0;
	std::vector<unsigned int> shader_ids = Shader::Get_Shader_ID_List();
	for (const auto& ID : shader_ids) {
		Shader* shader = Shader::Get_Shader(ID);
		shader->use(true);
		std::vector<Renderer*> renderers = Shader::Get_Shader_Renderer_List(ID);
		for (const auto& rend : renderers) {
			if (rend->Transparent()) {
				alpha_renderers.push_back(rend);
				float dist = glm::distance(Object()->Get_Transform()->Position(), rend->worldObject()->Get_Transform()->Position());
				glm::vec4 a_map = glm::vec4((float)i++, dist, 0.0f, 0.0f);
				alpha_object_idx.push_back(a_map);
				continue;
			}
			rend->Draw(dt);
		}

	}
}

void Camera::render_transparent(float dt)
{
	for (const auto& elem : alpha_object_idx) {
		int i = lround(elem.x);
		Renderer* rend = alpha_renderers[i];
		rend->Draw(dt);
	}
}

void Camera::render(float dt)
{
	m_framebuffer->Use(true);

	glClearColor(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	render_opaque(dt);
	render_transparent(dt);
	render_skybox(dt);

	m_framebuffer->Use(false);
}
