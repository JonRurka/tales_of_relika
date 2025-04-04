set(module "VoxelEngine")

include_directories(include)
set(${module}_headers
	include/stdafx.h
    include/ForwardDeclarations.h
    include/IVoxelBuilder_private.h
    include/shared_structures.h
    include/SmoothVoxelBuilder.h
    include/marchingcubes_arrays.h
    include/VoxelComputeProgram.h
    include/HeightmapGenerator.h
    include/ChunkProcessor_private.h
	
	include/std_includes.h
    include/IVoxelBuilder.h
    include/ChunkProcessor.h
    include/AsyncChunkProcessor.h
)

set(${module}_sources
    src/IVoxelBuilder_private.cpp
    src/SmoothVoxelBuilder.cpp
    src/marchingcubes_arrays.cpp
    src/VoxelComputeProgram.cpp
    src/HeightmapGenerator.cpp
    src/ChunkProcessor_private.cpp
	
    src/IVoxelBuilder.cpp
    src/ChunkProcessor.cpp
    src/AsyncChunkProcessor.cpp
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
	#vulkan-headers::vulkan-headers
	#GPUOpen::VulkanMemoryAllocator
)

target_link_libraries(${module} Compute Engine)


