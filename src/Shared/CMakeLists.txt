set(module "Shared")

include_directories(include)


set(${module}_headers
	include/Material_Types.h
)

set(${module}_sources
	src/Material_Types.cpp
)

add_library(${module} STATIC ${${module}_headers} ${${module}_sources})

target_link_libraries(${module} 
	ZLIB::ZLIB
	assimp::assimp 
	nlohmann_json::nlohmann_json 
	#glaze::glaze 
	boost::boost
	glm::glm
	glfw
	sol2::sol2
	#Bullet::Bullet
	Jolt::Jolt
	#OpenAL::OpenAL
	inih::inih
	imgui::imgui
	#vulkan-headers::vulkan-headers
	#GPUOpen::VulkanMemoryAllocator
	#rmlui
	#rmlui_debugger
	#rmlui_shell
)

target_link_libraries(${module} Compute VoxelEngine Engine)