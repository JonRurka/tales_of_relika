cmake_minimum_required(VERSION 3.21)
project(game CXX)

set(CMAKE_CXX_STANDARD 23)

add_compile_definitions(PHYSICS_BACKEND_BULLET=1)
add_compile_definitions(PHYSICS_BACKEND_JOLT=2)
add_compile_definitions(PHYSICS_BACKEND=PHYSICS_BACKEND_JOLT)

option(ASAN_ENABLED "Enable Address Sanitation" OFF)

if(ASAN_ENABLED)
	string(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}\n")
	add_compile_options(-fsanitize=address)
	add_compile_definitions(_DISABLE_STRING_ANNOTATION=1 _DISABLE_VECTOR_ANNOTATION=1)
	set(LDFLAGS "${LDFLAGS} -fsanitize=address")
endif() 


if (UNIX)
    add_compile_options(-pthread -fPIC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
	#set(CMAKE_CXX_COMPILER /opt/gcc-15/bin/g++)
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
#find_package(glaze REQUIRED)
find_package(Boost REQUIRED)
find_package(sol2 REQUIRED)
find_package(Bullet REQUIRED)
find_package(Jolt REQUIRED)
find_package(fmt REQUIRED)
#find_package(OpenAL REQUIRED)
find_package(inih REQUIRED)
find_package(imgui REQUIRED)


if (UNIX)
	set(OpenCL_SDK_Lib ${PROJECT_SOURCE_DIR}/lib/OpenCL_SDK/unix)
	LIST(APPEND OPENCL_LIBS 
		OpenCL libOpenCLSDK.a libOpenCLSDKCpp.a libOpenCLUtils.a libOpenCLUtilsCpp.a
	)
else()
	set(OpenCL_SDK_Lib ${PROJECT_SOURCE_DIR}/lib/OpenCL_SDK/win32)
	LIST(APPEND OPENCL_LIBS 
		OpenCL OpenCLUtils OpenCLUtilsCpp
	)
endif()
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

if (NDEBUG)
	set(RmlUi_LIBRARY ${PROJECT_SOURCE_DIR}/lib/RmlUi/Release)
	set(RmlUi_BINARY ${PROJECT_SOURCE_DIR}/bin/RmlUi/Release)
else()
	set(RmlUi_LIBRARY ${PROJECT_SOURCE_DIR}/lib/RmlUi/Debug)
	set(RmlUi_BINARY ${PROJECT_SOURCE_DIR}/bin/RmlUi/Debug)
endif()
link_directories(${RmlUi_LIBRARY})


add_subdirectory(src)


