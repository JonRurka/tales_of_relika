set(module "Compute")


include_directories(include)

set(${module}_headers
	include/ForwardDeclarations.h
	include/vulkan_utils.h
	include/ComputeEngine.h
	include/ComputeController_VK.h
	include/ComputeProgram_VK.h
	include/ComputeBuffer_VK.h
	include/InterfaceIncludes_private.h
	include/PlatformStructures_private.h
	include/ComputeInterface_private.h
	include/IComputeController_private.h
	include/IComputeProgram_private.h
	include/IComputeBuffer_private.h
	include/std_includes.h
	include/dynamic_compute.h
	include/InterfaceIncludes.h
	include/PlatformStructures.h
	include/IComputeBuffer.h
	include/IComputeProgram.h
	include/IComputeController.h
	include/ComputeInterface.h
)

set(${module}_sources
	src/vulkan_utils.cpp
	src/ComputeEngine.cpp
	src/ComputeController_VK.cpp
	src/ComputeProgram_VK.cpp
	src/ComputeBuffer_VK.cpp
	src/PlatformStructures.cpp
	src/IComputeBuffer.cpp
	src/IComputeProgram.cpp
	src/IComputeController.cpp
	src/ComputeInterface.cpp
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



