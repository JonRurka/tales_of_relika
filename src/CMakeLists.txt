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
	assimp::assimp 
	nlohmann_json::nlohmann_json 
	boost::boost
	Vulkan-1
	glm::glm
	glfw
	lua::lua
	Bullet::Bullet
	vulkan-headers::vulkan-headers
	vulkan-memory-allocator::vulkan-memory-allocator
)
target_link_libraries(${PROJECT_NAME} LuaCpp Compute VoxelEngine Engine Game)
