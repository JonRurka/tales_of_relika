#include "Camera.h"

#include "WorldObject.h"
#include "Transform.h"
#include "Framebuffer.h"
#include "Logger.h"
#include "Shader.h"
#include "Renderer.h"
#include "Graphics.h"

Camera* Camera::m_active_camera{nullptr};

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

void Camera::render(float dt)
{
	m_framebuffer->Use(true);

	glClearColor(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	std::vector<unsigned int> shader_ids = Shader::Get_Shader_ID_List();
	for (const auto& ID : shader_ids)
	{
		Shader* shader = Shader::Get_Shader(ID);
		shader->use(true);
		//shader->Bind_Textures();
		shader->Update_Source_Materials(dt);
		std::vector<Renderer*> renderers = Shader::Get_Shader_Renderer_List(ID);
		for (const auto& rend : renderers) {
			rend->Draw(dt);
		}
	}

	m_framebuffer->Use(false);
}
