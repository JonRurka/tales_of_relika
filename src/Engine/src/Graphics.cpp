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
#include "Engine.h"
#include "Input.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 600

#define SCREEN_VERT_SHADER "graphics::engine::screen::screen.vert"
#define SCREEN_FRAG_SHADER "graphics::engine::screen::screen.frag"

#define LINE_VERT_SHADER "graphics::engine::line::line.vert"
#define LINE_FRAG_SHADER "graphics::engine::line::line.frag"

Graphics* Graphics::m_instance{ nullptr };

unsigned int Graphics::CreateBufferGL(int size, const float* data, unsigned int usage)
{
	GLuint ret_val = 0;
	glGenBuffers(1, &ret_val);
	glBindBuffer(GL_ARRAY_BUFFER, ret_val);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), data, usage);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return ret_val;
}

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

	std::vector<glm::vec3> line_verts = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(1.0f,  0.0f,  0.0f),
	};

	m_screen_mesh = new Mesh();
	m_screen_mesh->Vertices(quad_verts);
	m_screen_mesh->TexCoords(quad_tex_coord);

	m_screen_shader = Shader::Create("screen_shader", SCREEN_VERT_SHADER, SCREEN_FRAG_SHADER);
	if (m_screen_shader == nullptr || !m_screen_shader->Initialized()) {
		Logger::LogFatal(LOG_POS("Initialize"), "Failed to create screen shader.");
		return;
	}


	m_line_mesh = new Mesh();
	m_line_mesh->Vertices(line_verts);

	m_line_shader = Shader::Create("line_shader", LINE_VERT_SHADER, LINE_FRAG_SHADER);
	if (m_line_shader == nullptr || !m_line_shader->Initialized()) {
		Logger::LogFatal(LOG_POS("Initialize"), "Failed to create line shader.");
		return;
	}


	Logger::LogInfo(LOG_POS("Initialize"), "Graphics Initialized");
	m_initialized = true;
}

void Graphics::Update(float dt)
{
	if (!m_initialized)
		return;

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
	if (!m_initialized)
		return;
	m_screen_shader->use(false);
	m_screen_shader->Set_Textures({
		{"screenTexture", tex}
	});
}

void Graphics::OnWindowResize(int width, int height)
{
	if (!m_initialized)
		return;
	m_width = width;
	m_height = height;

	for (const auto& tex : m_resize_textures) {
		tex->Resize(width, height);
	}
}

void Graphics::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Input::Instance()->key_callback(window, key, scancode, action, mods);	
}

void Graphics::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Input::Instance()->cursor_position_callback(window, xpos, ypos);
}

void Graphics::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	Input::Instance()->mouse_button_callback(window, button, action, mods);
}


void Graphics::draw_debug_ray(glm::vec3 start, glm::vec3 dir, glm::vec3 color, float duration)
{
	draw_debug_line(start, start + dir, color, duration);
}

void Graphics::draw_debug_line(glm::vec3 start, glm::vec3 stop, glm::vec3 color, float duration)
{
	float cur_time = (float)Utilities::Get_Time();
	DebugLines line{ start, stop, color, duration, cur_time };
	m_debug_lines.push_back(line);
}

void Graphics::render(float dt)
{
	if (!m_initialized)
		return;

	Camera* active_cam = Camera::Get_Active();
	if (active_cam == nullptr)
		return;

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	m_screen_shader->use(false);
	m_screen_shader->Bind_Textures();
	m_screen_mesh->Draw();


	DrawDebugRay(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0));
	DrawDebugRay(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
	DrawDebugRay(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1));

	// Debug Lines
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	m_line_shader->use(true);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	std::vector<DebugLines> tmp_debug_lines;
	tmp_debug_lines.reserve(m_debug_lines.size());

	glm::vec3 line_verts[2];
	for (const auto& line : m_debug_lines) {

		m_line_shader->SetVec3("color", line.color);

		line_verts[0] = line.start;
		line_verts[1] = line.stop;

		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float) * 3, line_verts, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_LINES, 0, 2);

		float cur_time = (float)Utilities::Get_Time();
		if (cur_time < (line.time + line.duration)) {
			tmp_debug_lines.push_back(line);
		}

	}

	m_debug_lines = tmp_debug_lines;

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Graphics::update_window_title(std::string title)
{
	glfwSetWindowTitle(m_window, title.c_str());
}

void Graphics::set_mouse_visibility(bool visible)
{
	int visibility_mode = visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
	glfwSetInputMode(m_window, GLFW_CURSOR, visibility_mode);
}
