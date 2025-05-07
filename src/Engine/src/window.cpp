#include "window.h"

#include <cstdio>
#include <iostream>

#include "opengl.h"

#include <Graphics.h>
#include <Engine.h>

#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

//#include <GL/wgl.h>

#include "resources.h"
#include <windows.h>
#include <windowsx.h>
#elif defined LINUX
#define GLFW_EXPOSE_NATIVE_GLX
#define GLFW_EXPOSE_NATIVE_X11
#include <GL/glx.h>
#include <GLFW/glfw3native.h>
#endif

#include "Logger.h"

window* window::m_instance{NULL};

namespace {
    static HMODULE s_module = NULL;

    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
        Graphics* user_ptr = (Graphics*)glfwGetWindowUserPointer(window);
        if (user_ptr == nullptr)
            return;
        user_ptr->OnWindowResize(width, height);
    }

    void APIENTRY glDebugOutput(GLenum source,
        GLenum type,
        unsigned int id,
        GLenum severity,
        GLsizei length,
        const char* message,
        const void* userParam)
    {
        // ignore non-significant error/warning codes
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

        std::cout << "---------------" << std::endl;
        std::cout << "Debug message (" << id << "): " << message << std::endl;

        switch (source)
        {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
        }
        std::cout << std::endl;

        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
        }
        std::cout << std::endl;

        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
        }
        std::cout << std::endl;
        std::cout << "---------------" << std::endl;
        std::cout << std::endl;
    }

    void static_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        Graphics* user_ptr = (Graphics*)glfwGetWindowUserPointer(window);
        if (user_ptr == nullptr)
            return;
        user_ptr->key_callback(window, key, scancode, action, mods);
    }

    void static_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
        Graphics* user_ptr = (Graphics*)glfwGetWindowUserPointer(window);
        if (user_ptr == nullptr)
            return;
        user_ptr->cursor_position_callback(window, xpos, ypos);
    }

    void static_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        Graphics* user_ptr = (Graphics*)glfwGetWindowUserPointer(window);
        if (user_ptr == nullptr)
            return;
        user_ptr->mouse_button_callback(window, button, action, mods);
    }

}

GLFWwindow* window::Create_Window(const char* title, int width, int height, void* user_obj)
{
    m_instance = this;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	m_window = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwSetWindowUserPointer(m_window, user_obj);
	if (m_window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);  // Enable vsync

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	glViewport(0, 0, width, height);

	glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSetKeyCallback(m_window, static_key_callback);
    glfwSetCursorPosCallback(m_window, static_cursor_position_callback);
    glfwSetMouseButtonCallback(m_window, static_mouse_button_callback);

    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, 1);
    glfwSetInputMode(m_window, GLFW_STICKY_MOUSE_BUTTONS, 1);

	int flags; 
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		// initialize debug output 
		Logger::LogDebug(LOG_POS("Create_Window"), "Debug context enabled.");
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

    load_module();

    return m_window;
}

void window::set_title(std::string title)
{
    glfwSetWindowTitle(m_window, title.c_str());
}

bool window::Should_Close()
{
	return glfwWindowShouldClose(m_window);
}

void window::load_module()
{
#ifdef WIN32
    if (!s_module)
    {
        s_module = LoadLibraryA("opengl32.dll");
        if (s_module) {
            printf("Found opengl32.dll\n");
        }
    }
#endif
}

#ifdef WIN32
void* window::sysGetProcAddress(const char* name)
{
    void* p = (void*)wglGetProcAddress(name);
    if (p == 0 || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) || (p == (void*)-1))
    {
        p = (void*)GetProcAddress(s_module, name);
    }

    return p;
}
#else
void* window::sysGetProcAddress(const char* name)
{
    void* p = (void*)glfwGetProcAddress(name);

    return p;
}
#endif
