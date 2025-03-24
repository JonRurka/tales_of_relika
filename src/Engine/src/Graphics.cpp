#include "Graphics.h"

#include "stb_image.h"
#include "window.h"
#include "Utilities.h"

#include "WorldObject.h"
#include "Renderer.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Logger.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 600

#define SCREEN_VERT_SHADER "graphics.engine.screen.screen.vert"
#define SCREEN_FRAG_SHADER "graphics.engine.screen.screen.frag"

Graphics* Graphics::m_instance{ nullptr };

void Graphics::Initialize()
{
	m_instance = this;

	m_width = DEFAULT_SCREEN_WIDTH;
	m_height = DEFAULT_SCREEN_HEIGHT;


	m_window = w.Create_Window("Learn OpenGL", m_width, m_height, this);

	

	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	//glFrontFace(GL_CCW);  

	// TODO: Per-material blending.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	std::vector<glm::vec3> quad_verts = {
		glm::vec3(-1.0f,  1.0f,  0.0f),
		glm::vec3(-1.0f, -1.0f,  0.0f),
		glm::vec3(1.0f, -1.0f,  0.0f),

		glm::vec3(-1.0f,  1.0f,  0.0f),
		glm::vec3(1.0f, -1.0f,  0.0f),
		glm::vec3(1.0f,  1.0f,  0.0f),
	};

	std::vector<glm::vec2> quad_tex_coord = {
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),

		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
	};

	m_screen_mesh = new Mesh();
	m_screen_mesh->Vertices(quad_verts);
	m_screen_mesh->TexCoords(quad_tex_coord);

	m_screen_shader = Shader::Create("screen_shader", SCREEN_VERT_SHADER, SCREEN_FRAG_SHADER);
	if (!m_screen_shader->Initialized()) {
		Logger::LogFatal(LOG_POS("Initialize"), "Failed to create screen shader.");
	}
}

void Graphics::Update(float dt)
{
	
	render(dt);


	/*glClearColor(m_clear_color.x, m_clear_color.y, m_clear_color.z, m_clear_color.w); // 0.2f, 0.3f, 0.3f, 1.0f
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

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
	}*/

	glfwSwapBuffers(m_window);
}

bool Graphics::Window_Should_Close()
{
	return w.Should_Close();
}

void Graphics::Set_Screen_FrameTexture(Texture* tex)
{
	m_screen_shader->use(false);
	m_screen_shader->Set_Textures({
		{"screenTexture", tex}
	});
}

void Graphics::OnWindowResize(int width, int height)
{
	m_width = width;
	m_height = height;

	for (const auto& tex : m_resize_textures) {
		tex->Resize(width, height);
	}
}

void Graphics::render(float dt)
{
	Camera* active_cam = Camera::Get_Active();
	if (active_cam == nullptr)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	m_screen_shader->use(false);
	m_screen_shader->Bind_Textures();
	m_screen_mesh->Draw();
}
