project("tales_of_relika")

include_directories(LuaCpp)
include_directories(Compute/include)
include_directories(VoxelEngine/include)
include_directories(Engine/include)
include_directories(Game/include)

add_subdirectory(LuaCpp)
add_subdirectory(Compute)
add_subdirectory(VoxelEngine)
add_subdirectory(Engine)
add_subdirectory(Game)

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
    exe_main.cpp
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_sources})

target_link_libraries(${PROJECT_NAME} 
	${PLATFORM_LIBRARIES}
	ZLIB::ZLIB
	assimp::assimp 
	nlohmann_json::nlohmann_json 
	boost::boost
	Vulkan::Vulkan
	glm::glm
	glfw
	spirv-cross-core
	spirv-cross-glsl
	spirv-cross-msl
	spirv-cross-reflect
	lua::lua
	Bullet::Bullet
	OpenAL::OpenAL
	#vulkan-headers::vulkan-headers
	nvpro_core
	#GPUOpen::VulkanMemoryAllocator
)
target_link_libraries(${PROJECT_NAME} LuaCpp Compute VoxelEngine Engine Game)
