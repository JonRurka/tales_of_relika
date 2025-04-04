#pragma once

// do not include glad

#include <string>

struct GLFWwindow;

class window
{
	friend class Graphics;
public:
	GLFWwindow* Create_Window(const char* title, int width, int height, void* user_obj = nullptr);

	bool Should_Close();

	static void* sysGetProcAddress(const char* name);

private:
	GLFWwindow* m_window;

	inline static const std::string LOG_LOC{ "WINDOW" };
};

