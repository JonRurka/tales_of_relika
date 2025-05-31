cmake_minimum_required(VERSION 3.24)
project(game CXX)

set(CMAKE_CXX_STANDARD 23)



if (UNIX)
    add_compile_options(-pthread -fPIC)
endif ()

find_package(ZLIB REQUIRED)
find_package(glm REQUIRED)
#find_package(VulkanHeaders REQUIRED)
#find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glew REQUIRED)
find_package(spirv-cross REQUIRED)
find_package(assimp REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Boost REQUIRED)
find_package(lua REQUIRED)
find_package(Bullet REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenAL REQUIRED)
find_package(inih REQUIRED)

set(OpenCL_SDK_Lib ${PROJECT_SOURCE_DIR}/lib/OpenCL_SDK/win32)
link_directories(${OpenCL_SDK_Lib})

find_package(Vulkan REQUIRED
COMPONENTS glslc glslangValidator shaderc_combined)
if(Vulkan_FOUND)
	message(STATUS "Vulkan library found!")
endif(Vulkan_FOUND)
 

include_directories(PUBLIC ${glfw3_INCLUDE_DIRS})
include_directories(PUBLIC ${glew_INCLUDE_DIRS})
include_directories(PUBLIC ${Vulkan_INCLUDE_DIRS})

#set(Vulkan_LIBRARY ${PROJECT_SOURCE_DIR}/lib/vulkan_1.3.239.0/Lib)
#link_directories(${Vulkan_LIBRARY})
message(STATUS "vulkan library: ${Vulkan_LIBRARY}")

if (NDEBUG)
	set(nvpro_core_LIBRARY ${PROJECT_SOURCE_DIR}/lib/nvpro_core/Release)
else()
	set(nvpro_core_LIBRARY ${PROJECT_SOURCE_DIR}/lib/nvpro_core/Debug)
endif()

link_directories(${nvpro_core_LIBRARY})

add_subdirectory(src)


