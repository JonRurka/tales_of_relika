#include "window.h"

#include <cstdio>
#include <iostream>
#include <sstream>

#include "opengl.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"

#include <Graphics.h>
#include <Engine.h>


#define GLFW_HEADLESS false

#ifdef WIN32
#include <GLFW/glfw3native.h>

//#include <GL/wgl.h>

#include "resources.h"
#include <windows.h>
#include <windowsx.h>
#else
#include <GL/glx.h>
#include <GLFW/glfw3native.h>
#endif

#include "Logger.h"

window* window::m_instance{nullptr};

namespace {
#ifdef WIN32
    static HMODULE s_module = NULL;
#endif

    std::string toString(std::ostream& str)
    {
        std::ostringstream ss;
        ss << str.rdbuf();
        return ss.str();
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

        std::stringstream  error_msg;

        error_msg << "\n---------------" << std::endl;
        error_msg << "Debug message (" << id << "): " << message << std::endl;

        switch (source)
        {
        case GL_DEBUG_SOURCE_API:             error_msg << "Source: API";
            //assert(false);
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   error_msg << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: error_msg << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     error_msg << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     error_msg << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           error_msg << "Source: Other"; break;
        }
        error_msg << std::endl;

        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR:               error_msg << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: error_msg << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  error_msg << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         error_msg << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         error_msg << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              error_msg << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          error_msg << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           error_msg << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               error_msg << "Type: Other"; break;
        }
        error_msg << std::endl;

        bool should_report = false;
        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:         error_msg << "Severity: high";
            should_report = true;
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:       error_msg << "Severity: medium";
            should_report = true;
            break;
        case GL_DEBUG_SEVERITY_LOW:          error_msg << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: error_msg << "Severity: notification"; break;
        }

        error_msg << std::endl;
        error_msg << "---------------" << std::endl;
        error_msg << std::endl;

        if (should_report)
        {
            Logger::LogError("GL_LOG", "%s", toString(error_msg).c_str());
        }
    }

    void static_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        Graphics* user_ptr = (Graphics*)glfwGetWindowUserPointer(window);
        if (user_ptr == nullptr)
            return;
        user_ptr->key_callback(window, key, scancode, action, mods);
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        Graphics* user_ptr = (Graphics*)glfwGetWindowUserPointer(window);
        if (user_ptr == nullptr)
            return;
        if (width == 0 || height == 0)
            return;
        user_ptr->OnWindowResize(window, width, height);
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

    void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        Graphics* user_ptr = (Graphics*)glfwGetWindowUserPointer(window);
        if (user_ptr == nullptr)
            return;
        user_ptr->scroll_callback(window, xoffset, yoffset);
    }

}

GLFWwindow* window::Create_Window(const char* title, int width, int height, void* user_obj)
{
    m_instance = this;
    m_headless = GLFW_HEADLESS;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    //glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    if (m_headless)
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    refresh_ui_scale();

    int scaled_width = (width * m_main_scale);
    int scaled_height = (height * m_main_scale);

	m_window = glfwCreateWindow(scaled_width, scaled_height, title, NULL, NULL);
	if (m_window == NULL)
	{
        Logger::LogFatal(LOG_POS("Create_Window"), "Failed to create GLFW window");
		glfwTerminate();
        return m_window;
	}

    glfwSetWindowUserPointer(m_window, user_obj);

	glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);  // Enable vsync

	if (!Init_Glad())
	{
		Logger::LogFatal(LOG_POS("Create_Window"), "Failed to initialize GLAD");
        glfwTerminate();
        return m_window;
	}

	glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSetKeyCallback(m_window, static_key_callback);
    glfwSetCursorPosCallback(m_window, static_cursor_position_callback);
    glfwSetMouseButtonCallback(m_window, static_mouse_button_callback);
    glfwSetScrollCallback(m_window, ScrollCallback);

    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, 1);
    glfwSetInputMode(m_window, GLFW_STICKY_MOUSE_BUTTONS, 1);

	int flags; 
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		// initialize debug output 
		//Logger::LogDebug(LOG_POS("Create_Window"), "Debug context enabled.");
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#if GL_MINOR >= 3
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif
	}

    load_module();

#if WIN32
    if (!m_headless)
    {
        m_cur_context = glfwGetWGLContext(m_window);
        m_cur_display = GetDC(glfwGetWin32Window(m_window));
    }
#else
    m_cur_context = glfwGetGLXContext(m_window);
    m_cur_display = glfwGetX11Display();
#endif

    return m_window;
}

void window::refresh_ui_scale()
{
    if (!m_headless)
    {
        m_main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    }
    else
    {
        m_main_scale = 1.0;
    }
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
