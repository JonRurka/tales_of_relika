#include "opengl.h"

#include "Logger.h"

namespace {
	bool glad_initialized{ false };
}

bool Init_Glad()
{
	if (!glad_initialized) {
		bool inited = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (!inited) {
			Logger::LogFatal("Init_Glad", "glad GL Loader failed.");
		}
		glad_initialized = inited;
	}
	return glad_initialized;
}
