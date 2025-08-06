#pragma once

#define GL_MAJOR 4
#define GL_MINOR 5

#include "glad.h"

#if WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#else
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif


#include <GLFW/glfw3.h>

bool Init_Glad();
