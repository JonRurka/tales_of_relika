project("tales_of_relika")

option(BUILD_SHARED_LIBS "CMake standard option. Choose whether to build shared RmlUi libraries." OFF)
option(SOURCE_RMLUI "Include RMLUI as source rather than linked library." ON)
option(VULKAN_PLATFROM "Include Vulkan platform for compute." OFF)

add_compile_definitions(RMLUI_STATIC_LIB)
add_compile_definitions(RMLUI_GL3_CUSTOM_LOADER="opengl.h")

if (SERVER_BUILD)
	add_compile_definitions(SERVER_BUILD=1)
endif()

if (NO_UI)
	add_compile_definitions(NO_UI=1)
endif()


add_compile_definitions(CL_TARGET_OPENCL_VERSION=210)
add_compile_definitions(CL_HPP_TARGET_OPENCL_VERSION=210)


add_compile_definitions(NOMINMAX)

if (VULKAN_PLATFROM)
	add_compile_definitions(VULKAN_COMPUTE_PLATFROM)
	LIST(APPEND VULKAN_LIBS 
		Vulkan::Vulkan
		nvpro_core
	)
else()
	
endif()

if (NOT NO_UI)
	if(SOURCE_RMLUI)
		LIST(APPEND RMLUI_LIBS 
			UI 
			UI_debugger
		)
	else()
		LIST(APPEND RMLUI_LIBS 
			rmlui 
			rmlui_debugger
		)
	endif()
endif()

#include_directories(LuaCpp)
include_directories(Compute/include)
include_directories(VoxelEngine/include)
include_directories(Engine/include)
include_directories(Shared/include)

# include tracy header project wide.
include_directories(Shared/thirdparty/tracy_client)

if (NOT SERVER_BUILD)
	include_directories(Game/include)
endif()

include_directories(Server/include)

if (NOT NO_UI)
	include_directories(UI/include)
endif()

## Your-external "mylib", add GLOBAL if the imported library is located in directories above the current.
#add_library(rmlui_bin SHARED IMPORTED )
#set_target_properties(rmlui_bin PROPERTIES 
#	IMPORTED_LOCATION ${RmlUi_BINARY}/rmlui.dll
#	IMPORTED_IMPLIB ${RmlUi_LIBRARY}/rmlui.lib
#)
#
#add_library(rmlui_debugger_bin SHARED IMPORTED)
#set_target_properties(rmlui_debugger_bin PROPERTIES 
#	IMPORTED_LOCATION ${RmlUi_BINARY}/rmlui_debugger.dll 
#	IMPORTED_IMPLIB ${RmlUi_LIBRARY}/rmlui_debugger.lib
#)

#add_subdirectory(LuaCpp)
add_subdirectory(Compute)
add_subdirectory(VoxelEngine)
add_subdirectory(Engine)
add_subdirectory(Shared)
add_subdirectory(Server)

if (NOT SERVER_BUILD)
	add_subdirectory(Game)
endif()

if (NOT NO_UI)
	add_subdirectory(UI)
endif()

add_subdirectory(Server_Exe)

# Add X11 for screenshot support on linux
if(UNIX)
  find_package(X11 REQUIRED)
  LIST(APPEND PLATFORM_LIBRARIES ${X11_LIBRARIES})
endif()

# Make Windows compilation somewhat faster, and link with Dbghelp for
# stack trace information
if(WIN32)
  LIST(APPEND PLATFORM_LIBRARIES DbgHelp)
endif()


set(${PROJECT_NAME}_sources
    game_exe_main.cpp
)


if (NOT SERVER_BUILD)
	add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_sources})

	target_link_libraries(${PROJECT_NAME} 
		${PLATFORM_LIBRARIES}
		${RMLUI_LIBS}
		${VULKAN_LIBS}
		ZLIB::ZLIB
		assimp::assimp 
		nlohmann_json::nlohmann_json 
		#glaze::glaze
		boost::boost
		glm::glm
		glfw
		spirv-cross-core
		spirv-cross-glsl
		spirv-cross-msl
		spirv-cross-reflect
		sol2::sol2
		#Bullet::Bullet
		Jolt::Jolt
		#OpenAL::OpenAL
		inih::inih
		imgui::imgui
		#vulkan-headers::vulkan-headers
		
		#GPUOpen::VulkanMemoryAllocator
		#rmlui
		#rmlui_debugger
		#rmlui_shell
	)


	target_link_libraries(${PROJECT_NAME} Compute VoxelEngine Engine Shared Game Server UI_Backend)
	
	if(ASAN_ENABLED)
		target_link_libraries(${PROJECT_NAME} -fsanitize=address)
	endif()
endif()