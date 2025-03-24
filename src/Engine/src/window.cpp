#include "window.h"

#include <cstdio>
#include <iostream>

#include <Graphics.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

	static_cast<Graphics*>(glfwGetWindowUserPointer(window))->OnWindowResize(width, height);
}

GLFWwindow* window::Create_Window(const char* title, GLsizei width, GLsizei height, void* user_obj)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	m_window = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwSetWindowUserPointer(m_window, user_obj);
	if (m_window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(m_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	glViewport(0, 0, width, height);

	glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);


    return m_window;
}

bool window::Should_Close()
{
	return glfwWindowShouldClose(m_window);
}
