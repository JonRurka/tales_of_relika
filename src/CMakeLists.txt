project("tales_of_relika")

option(BUILD_SHARED_LIBS "CMake standard option. Choose whether to build shared RmlUi libraries." OFF)
add_compile_definitions(RMLUI_STATIC_LIB)
add_compile_definitions(RMLUI_GL3_CUSTOM_LOADER="glad.h")

#include_directories(LuaCpp)
include_directories(Compute/include)
include_directories(VoxelEngine/include)
include_directories(Engine/include)
include_directories(Game/include)
include_directories(Server/include)
include_directories(UI/include)


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
add_subdirectory(Server)
add_subdirectory(Game)
add_subdirectory(UI)
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

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_sources})

target_link_libraries(${PROJECT_NAME} 
	${PLATFORM_LIBRARIES}
	ZLIB::ZLIB
	assimp::assimp 
	nlohmann_json::nlohmann_json 
	glaze::glaze
	boost::boost
	Vulkan::Vulkan
	glm::glm
	glfw
	spirv-cross-core
	spirv-cross-glsl
	spirv-cross-msl
	spirv-cross-reflect
	sol2::sol2
	Bullet::Bullet
	OpenAL::OpenAL
	inih::inih
	imgui::imgui
	#vulkan-headers::vulkan-headers
	nvpro_core
	#GPUOpen::VulkanMemoryAllocator
	rmlui
	#rmlui_debugger
	#rmlui_shell
)


target_link_libraries(${PROJECT_NAME} Compute VoxelEngine Engine Game Server UI_debugger UI_Backend)
