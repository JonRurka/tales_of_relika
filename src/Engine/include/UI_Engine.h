#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "RmlUi_Platform_GLFW.h"
#include "RmlUi_Renderer_GL3.h"

class Graphics;
struct GLFWwindow;

namespace Rml {
	class Context;
	class ElementDocument;
}

#define DEFINE_UI_EVENT(_event_name_, _target_class_, _target_func_) \
class _event_name_ : public Rml::EventListener						 \
{																	 \
public:																 \
	_event_name_(_target_class_* target) : m_target{ target } {}	 \
	void ProcessEvent(Rml::Event& event) override {					 \
		m_target->_target_func_();									 \
	}																 \
private:															 \
	_target_class_* m_target{ nullptr };							 \
};

typedef std::shared_ptr<Rml::ElementDocument> ElementDocument;

class UI_Engine {
	friend class Graphics;
public:

	static UI_Engine& Instance() 
	{ 
		static UI_Engine inst;
		return inst; 
	}

	bool Init();

	void Load_Font(std::string resource_name);

	bool Document_Exists(std::string name);

	std::shared_ptr<Rml::ElementDocument> Load_Document_Resource(std::string name, std::string resource_name);

	std::shared_ptr<Rml::ElementDocument> Load_Document_File(std::string name, std::string file_path);

	std::shared_ptr<Rml::ElementDocument> Get_Document(std::string name);

	void Accept_Input(bool val) { m_accept_input = true; }

	void Display(std::string doc_name);

	void Update();

	void Shutdown();

private:

	std::unique_ptr<SystemInterface_GLFW> m_system_interface;
	std::unique_ptr<RenderInterface_GL3> m_render_interface;

	bool m_context_dimensions_dirty{true};
	Rml::Context* m_context{nullptr};

	bool m_initialized{ false };
	int m_glfw_active_modifiers{ 0 };

	int m_accept_input{ false };

	std::string m_ui_data_root;

	std::unordered_map<std::string, std::shared_ptr<Rml::ElementDocument>> m_documents;

	UI_Engine() = default;

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