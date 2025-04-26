set(module "Compute")


include_directories(include)
include_directories(include/Vulkan)
include_directories(include/Vulkan/nvpro_core/nvp)
include_directories(include/Vulkan/nvpro_core)

include_directories(include/OpenCL)

set(${module}_headers
	include/ForwardDeclarations.h
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
	
	include/Vulkan/nvpro_core/nvgl/extensions_gl.hpp
	include/Vulkan/vk_mem_alloc.h
	include/Vulkan/vulkan_utils.h
	include/Vulkan/ComputeEngine_VK.h
	include/Vulkan/ComputeController_VK.h
	include/Vulkan/ComputeProgram_VK.h
	include/Vulkan/ComputeBuffer_VK.h
	
	include/OpenCL/ComputeEngine_OCL.h
	#include/OpenCL/ComputeController.h
	include/OpenCL/ComputeController_OCL.h
	include/OpenCL/ComputeProgram_OCL.h
	include/OpenCL/ComputeBuffer_OCL.h
	#include/OpenCL/OCL_forwardDeclarations.h
	#include/OpenCL/ProgramBuilder.h
)

set(${module}_sources
	src/PlatformStructures.cpp
	src/IComputeBuffer.cpp
	src/IComputeProgram.cpp
	src/IComputeController.cpp
	src/ComputeInterface_private.cpp
	src/ComputeInterface.cpp
	
	#include/nvpro_core/nvgl/extensions_gl.cpp
	src/Vulkan/vulkan_utils.cpp
	src/Vulkan/ComputeEngine_VK.cpp
	src/Vulkan/ComputeController_VK.cpp
	src/Vulkan/ComputeProgram_VK.cpp
	src/Vulkan/ComputeBuffer_VK.cpp
	
	src/OpenCL/ComputeEngine_OCL.cpp
	#src/OpenCL/ComputeController.cpp
	src/OpenCL/ComputeController_OCL.cpp
	src/OpenCL/ComputeProgram_OCL.cpp
	src/OpenCL/ComputeBuffer_OCL.cpp
	#src/OpenCL/ProgramBuilder.cpp
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



