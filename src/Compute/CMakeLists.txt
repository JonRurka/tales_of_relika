set(module "Compute")


include_directories(include)
include_directories(include/nvpro_core/nvp)
include_directories(include/nvpro_core)

set(${module}_headers
	include/ForwardDeclarations.h
	include/vk_mem_alloc.h
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
	
	include/nvpro_core/nvgl/extensions_gl.hpp
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
	src/ComputeInterface_private.cpp
	src/ComputeInterface.cpp
	
	#include/nvpro_core/nvgl/extensions_gl.cpp
)

if(WIN32)
  #target_compile_definitions(nvpro_core PRIVATE WIN32_LEAN_AND_MEAN)
  LIST(APPEND PLATFORM_LIBRARIES DbgHelp)
endif()

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
	nvpro_core
	fmt::fmt
)



