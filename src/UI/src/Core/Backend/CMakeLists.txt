# RMLUI_CMAKE_MINIMUM_VERSION_RAISE_NOTICE:
# Using absolute paths to prevent improper interpretation of relative paths. Relative paths can be used once the minimum
# CMake version is greater or equal than CMake 3.13
target_sources(${module} PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/PlatformExtensions.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/PlatformExtensions.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/RendererExtensions.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/RendererExtensions.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/RmlUi_Backend_GLFW_GL3.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/RmlUi_Include_GL3.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/RmlUi_Platform_GLFW.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/RmlUi_Platform_GLFW.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/RmlUi_Renderer_GL3.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/RmlUi_Renderer_GL3.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/FontEngineBitmap.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/FontEngineBitmap.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/FontEngineInterfaceBitmap.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/FontEngineInterfaceBitmap.cpp"
)
