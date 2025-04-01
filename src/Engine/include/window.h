#pragma once

#include "opengl.h"
#include <string>

class window
{
	friend class Graphics;
public:
	GLFWwindow* Create_Window(const char* title, GLsizei width, GLsizei height, void* user_obj = nullptr);

	bool Should_Close();

private:
	GLFWwindow* m_window;

	inline static const std::string LOG_LOC{ "WINDOW" };
};

