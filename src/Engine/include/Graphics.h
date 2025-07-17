#pragma once

#include "window.h"

#include "imgui.h"

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



struct GLFWwindow;
class Texture;
class Mesh;
class Shader;
class Engine;
class Input;
class UI_Engine;

class Graphics
{
	friend class Engine;
	friend class Input;
public:

	struct Render_Options {
	public:
		bool transparency_enabled{ false };
	};

	static Graphics* Instance() { return m_instance; }

	static int Width() { return m_instance->m_width; }
	static int Height() { return m_instance->m_height; }


	static void DrawDebugRay(glm::vec3 start, glm::vec3 dir, glm::vec3 color, float duration = 0.0f) { m_instance->draw_debug_ray(start, dir, color, duration); }
	static void DrawDebugLine(glm::vec3 start, glm::vec3 stop, glm::vec3 color, float duration = 0.0f) { m_instance->draw_debug_line(start, stop, color, duration); }

	static void Update_Window_Title(std::string title) { m_instance->update_window_title(title); }

	static unsigned int CreateBufferGL(int size, const void* data, unsigned int usage);

	void Initialize();
	void Update(float dt);

	bool Window_Should_Close();

	glm::vec4 Clear_Color() { return m_clear_color; }

	void Clear_Color(glm::vec4 value) { m_clear_color = value; }

	void Viewport(int x, int y, int width, int height);

	GLFWwindow* Get_GLFW_Window() { return m_window; }

	static ImGuiIO& ImGUI_IO() { return m_instance->m_io; }

	static UI_Engine* UI() { return m_instance->m_UI; }

	const Render_Options Active_Options() { return m_active_options; }

	void Set_Screen_FrameTexture(Texture* tex);
	

	void OnWindowResize(GLFWwindow* window, int width, int height);

	void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/);

	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

	void scroll_callback(GLFWwindow*, double, double yoffset);

	void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/);

	void Register_Resize_Tex(Texture* tex);

	void Remove_Resize_Tex(Texture* tex);

	bool Render_ImgUI();

private:

	struct DebugLines {
		glm::vec3 start; 
		glm::vec3 stop; 
		glm::vec3 color;
		float duration;

		float time;
	};

	float deltaTime = 0;
	GLFWwindow* m_window{nullptr};
	window w;
	int m_width{ 0 };
	int m_height{ 0 };
	static Graphics* m_instance;

	ImGuiIO m_io;
	UI_Engine* m_UI{ nullptr };

	bool m_initialized{ false };
	bool m_mouse_visible{ true };

	Mesh* m_screen_mesh{nullptr};
	Shader* m_screen_shader{ nullptr };

	Mesh* m_line_mesh{ nullptr };
	Shader* m_line_shader{ nullptr };

	Render_Options m_active_options;

	std::vector<Texture*> m_resize_textures;
	
	glm::vec4 m_clear_color{ glm::vec4(0.2f, 0.3f, 0.3f, 1.0f) };

	std::vector<DebugLines> m_debug_lines;

	void draw_debug_ray(glm::vec3 start, glm::vec3 dir, glm::vec3 color, float duration = 0.0f);
	void draw_debug_line(glm::vec3 start, glm::vec3 stop, glm::vec3 color, float duration = 0.0f);

	void render(float dt);

	void update_window_title(std::string title);

	void set_mouse_visibility(bool visible);

	bool get_mouse_visibility() { return m_mouse_visible; }

	inline static const std::string LOG_LOC{ "GRAPHICS" };
};

