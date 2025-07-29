#pragma once

// do not include glad

#include <string>

struct GLFWwindow;

class window
{
	friend class Graphics;
public:

	GLFWwindow* Create_Window(const char* title, int width, int height, void* user_obj = nullptr);

	void refresh_ui_scale();

	float window_scale() { return m_main_scale; }

	void set_title(std::string title);

	bool Should_Close();

	static void* sysGetProcAddress(const char* name);
	static void load_module();

	static GLFWwindow* glfw_window() { return m_instance->m_window; }


	static void* Get_Context() { return m_instance->m_cur_context; }
	static void* Get_Display() { return m_instance->m_cur_display; }



	static bool Has_Window() 
	{
		if (m_instance == nullptr)
			return false;
		if (m_instance->m_window == nullptr)
			return false;
		return true;
	}

private:
	GLFWwindow* m_window{nullptr};

	void* m_cur_context{ nullptr };
	void* m_cur_display{ nullptr };

	float m_main_scale{ 0 };
	static window* m_instance;

	inline static const std::string LOG_LOC{ "WINDOW" };
};

