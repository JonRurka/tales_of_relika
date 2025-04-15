set(module "Game")

include_directories(include)

set(${module}_headers
	include/game_imp.h
	include/Game.h
	include/Game_Resources.h
	include/Standard_Material.h
	include/Opaque_Chunk_Material.h
	
	include/Test_Scene.h
	include/Voxel_Test_Scene.h
	
	include/Editor_Camera_Control.h
)

set(${module}_sources
	src/Game.cpp
	src/Game_Resources.cpp
	src/Standard_Material.cpp
	src/Opaque_Chunk_Material.cpp
	
	src/Test_Scene.cpp
	src/Voxel_Test_Scene.cpp
	
	src/Editor_Camera_Control.cpp
)

add_library(${module} STATIC ${${module}_headers} ${${module}_sources})

target_link_libraries(${module} 
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
	#GPUOpen::VulkanMemoryAllocator
)

target_link_libraries(${module} LuaCpp Compute VoxelEngine Engine)





