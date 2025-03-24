set(module "Game")

include_directories(include)

set(${module}_headers
	include/game_imp.h
	include/Game.h
	include/Test_Scene.h
	include/Game_Resources.h
	include/Standard_Material.h
)

set(${module}_sources
	src/Game.cpp
	src/Test_Scene.cpp
	src/Game_Resources.cpp
	src/Standard_Material.cpp
)

add_library(${module} STATIC ${${module}_headers} ${${module}_sources})

target_link_libraries(${module} 
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

target_link_libraries(${module} LuaCpp Compute VoxelEngine Engine)





