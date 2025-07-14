#pragma once

#include <string>
#include <unordered_map>

class SystemInterface_GLFW;
class RenderInterface_GL3;
class Graphics;
struct GLFWwindow;

namespace Rml {
	class Context;
	class ElementDocument;
}

class UI_Engine {
	friend class Graphics;
public:

	UI_Engine();

	bool Init();

	void Load_Font(std::string resource_name);

	bool Document_Exists(std::string name);

	Rml::ElementDocument* Load_Document_Resource(std::string name, std::string resource_name);

	Rml::ElementDocument* Load_Document_File(std::string name, std::string file_path);

	void Display(std::string doc_name);

	void Update();

	void Shutdown();

	static UI_Engine* Instance() { return m_instance; }

private:

	static UI_Engine* m_instance;

	SystemInterface_GLFW* m_system_interface;
	RenderInterface_GL3* m_render_interface;

	bool m_context_dimensions_dirty{true};
	Rml::Context* m_context{nullptr};

	bool m_initialized{ false };
	int m_glfw_active_modifiers{ 0 };

	std::string m_ui_data_root;

	std::unordered_map<std::string, Rml::ElementDocument*> m_documents;

	bool KeyCallback(GLFWwindow* window, int glfw_key, int scancode, int glfw_action, int glfw_mods);

	void CharCallback(GLFWwindow* window, unsigned int codepoint);

	void CursorEnterCallback(GLFWwindow* window, int entered);

	void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

	bool MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	bool ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

	void WindowContentScaleCallback(GLFWwindow* window, float xscale, float yscale);

	inline static const std::string LOG_LOC{ "UI_ENGINE" };
};