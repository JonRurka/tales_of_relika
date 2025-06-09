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
	include/Test_OpenCL_Scene.h
	include/VoxelWorld_Scene.h
	include/GameClient.h
	include/Client_Server_Interface.h
	
	include/Editor_Camera_Control.h
	include/LocalPlayerCharacter.h
	include/NetPlayerCharacter.h
	include/NetPlayerManager.h
	
	include/Stitch_VBO.h
	include/WorldGenController.h
	include/TerrainChunk.h
	include/Material_Types.h
)

set(${module}_sources
	src/Game.cpp
	src/Game_Resources.cpp
	src/Standard_Material.cpp
	src/Opaque_Chunk_Material.cpp
	
	src/Test_Scene.cpp
	src/Voxel_Test_Scene.cpp
	src/Test_OpenCL_Scene.cpp
	src/VoxelWorld_Scene.cpp
	src/GameClient.cpp
	src/Client_Server_Interface.cpp
	
	src/Editor_Camera_Control.cpp
	src/LocalPlayerCharacter.cpp
	src/NetPlayerCharacter.cpp
	src/NetPlayerManager.cpp
	
	src/Stitch_VBO.cpp
	src/WorldGenController.cpp
	src/TerrainChunk.cpp
	src/Material_Types.cpp
)

add_library(${module} STATIC ${${module}_headers} ${${module}_sources})

target_link_libraries(${module} 
	ZLIB::ZLIB
	assimp::assimp 
	nlohmann_json::nlohmann_json 
	glaze::glaze 
	boost::boost
	Vulkan::Vulkan
	glm::glm
	glfw
	sol2::sol2
	Bullet::Bullet
	OpenAL::OpenAL
	inih::inih
	#vulkan-headers::vulkan-headers
	#GPUOpen::VulkanMemoryAllocator
)

target_link_libraries(${module} Compute VoxelEngine Engine Server)





