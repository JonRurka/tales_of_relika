set(module "Engine")


include_directories(include)
include_directories(include/RmlUi)

set(${module}_headers
	include/glad.h
	include/khrplatform.h
	include/imgui_impl_glfw.h
	include/imgui_impl_opengl3.h
	include/imgui_impl_opengl3_loader.h
	include/stb_image.h

	include/game_engine.h
	include/Logger.h
	include/Framebuffer.h
	include/Camera.h
	include/Component.h
	include/Resources.h
	include/Cube_Material.h
	include/Graphics.h
	include/Engine.h
	include/input_events.h
	include/Input.h
	#include/Raycast.h
	include/Physics.h
	include/Scene.h
	include/Light_Material.h
	include/Material.h
	include/Mesh.h
	include/MeshRenderer.h
	include/Model.h
	include/opengl.h
	include/Renderer.h
	include/Shader.h
	include/Texture.h
	include/Cubemap.h
	include/Transform.h
	include/Utilities.h
	include/window.h
	include/WorldObject.h
	include/GPUSort.h
	
	include/LuaEngine.h
	include/UI_Engine.h
	
	include/Light.h
	include/Collider.h
	include/BoxCollider.h
	include/SphereCollider.h
	include/CapsuleCollider.h
	include/MeshCollider.h
)

set(${module}_sources
	src/glad.c
	src/imgui_impl_glfw.cpp
	src/imgui_impl_opengl3.cpp
	src/def_stb_image.cpp

	src/Logger.cpp
	src/Framebuffer.cpp
	src/Camera.cpp
	src/Component.cpp
	src/Resources.cpp
	src/Cube_Material.cpp
	src/Graphics.cpp
	src/Input.cpp
	#src/Raycast.cpp
	src/Physics.cpp
	src/Engine.cpp
	src/Scene.cpp
	src/Light_Material.cpp
	src/Material.cpp
	src/Mesh.cpp
	src/MeshRenderer.cpp
	src/Model.cpp
	src/Renderer.cpp
	src/Shader.cpp
	src/Texture.cpp
	src/Cubemap.cpp
	src/Transform.cpp
	src/Utilities.cpp
	src/window.cpp
	src/WorldObject.cpp
	src/GPUSort.cpp
	
	src/LuaEngine.cpp
	src/UI_Engine.cpp
	
	src/Light.cpp
	src/Collider.cpp
	src/BoxCollider.cpp
	src/SphereCollider.cpp
	src/CapsuleCollider.cpp
	src/MeshCollider.cpp
)

add_library(${module} STATIC ${${module}_headers} ${${module}_sources})

target_link_libraries(${module} 
	ZLIB::ZLIB
	assimp::assimp 
	nlohmann_json::nlohmann_json 
	boost::boost
	Vulkan::Vulkan
	glm::glm
	glfw
	spirv-cross-core
	spirv-cross-glsl
	spirv-cross-msl
	spirv-cross-reflect
	sol2::sol2
	Bullet::Bullet
	OpenAL::OpenAL
	imgui::imgui
	#vulkan-headers::vulkan-headers
	#GPUOpen::VulkanMemoryAllocator
	#rmlui
	#rmlui_debugger
	#rmlui_shell
	#rmlui_bin rmlui_debugger_bin
)

target_link_libraries(${module} Compute VoxelEngine UI UI_debugger)



