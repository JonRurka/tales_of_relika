#pragma once

#include "window.h"

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

	void Initialize();
	void Update(float dt);

	bool Window_Should_Close();

	glm::vec4 Clear_Color() { return m_clear_color; }

	void Clear_Color(glm::vec4 value) { m_clear_color = value; }

	GLFWwindow* Get_GLFW_Window() { return m_window; }

	const Render_Options Active_Options() { return m_active_options; }

	void Set_Screen_FrameTexture(Texture* tex);
	

	void OnWindowResize(int width, int height);

	void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/);

	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

	void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/);

private:

	float deltaTime = 0;
	GLFWwindow* m_window{nullptr};
	window w;
	int m_width{ 0 };
	int m_height{ 0 };
	static Graphics* m_instance;

	Mesh* m_screen_mesh{nullptr};
	Shader* m_screen_shader{ nullptr };

	Render_Options m_active_options;

	std::vector<Texture*> m_resize_textures;
	
	glm::vec4 m_clear_color{ glm::vec4(0.2f, 0.3f, 0.3f, 1.0f) };


	void render(float dt);

	void set_mouse_visibility(bool visible);

	inline static const std::string LOG_LOC{ "GRAPHICS" };
};

