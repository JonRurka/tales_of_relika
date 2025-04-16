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

set(${PROJECT_NAME}_sources
    exe_main.cpp
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_sources})

target_link_libraries(${PROJECT_NAME} 
	ZLIB::ZLIB
	assimp::assimp 
	nlohmann_json::nlohmann_json 
	boost::boost
	Vulkan::Vulkan
	glm::glm
	glfw
	lua::lua
	Bullet::Bullet
	OpenAL::OpenAL
	#vulkan-headers::vulkan-headers
	nvpro_core
	#GPUOpen::VulkanMemoryAllocator
)
target_link_libraries(${PROJECT_NAME} LuaCpp Compute VoxelEngine Engine Game)
