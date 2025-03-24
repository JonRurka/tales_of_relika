cmake_minimum_required(VERSION 3.24)
project(game CXX)

set(CMAKE_CXX_STANDARD 23)

if (UNIX)
    add_compile_options(-pthread -fPIC)
endif ()

find_package(glm REQUIRED)
find_package(VulkanHeaders REQUIRED)
find_package(vulkan-memory-allocator CONFIG REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Boost REQUIRED)
find_package(lua REQUIRED)
find_package(Bullet REQUIRED)

include_directories(PUBLIC ${glfw3_INCLUDE_DIRS})

set(Vulkan_LIBRARY ${PROJECT_SOURCE_DIR}/lib/vulkan_1.3.239.0/Lib)
link_directories(${Vulkan_LIBRARY})

add_subdirectory(src)


