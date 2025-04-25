#pragma once

// do not include glad

#include <string>

struct GLFWwindow;

class window
{
	friend class Graphics;
public:

	GLFWwindow* Create_Window(const char* title, int width, int height, void* user_obj = nullptr);

	void set_title(std::string title);

	bool Should_Close();

	static void* sysGetProcAddress(const char* name);
	static void load_module();

	static GLFWwindow* glfw_window() { return m_instance->m_window; }

private:
	GLFWwindow* m_window;
	static window* m_instance;

	inline static const std::string LOG_LOC{ "WINDOW" };
};

