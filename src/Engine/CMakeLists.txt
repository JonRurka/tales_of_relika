set(module "Engine")


include_directories(include)

set(${module}_headers
	include/game_engine.h
	include/Logger.h
	include/Framebuffer.h
	include/Camera.h
	include/Component.h
	include/Resources.h
	include/Cube_Material.h
	include/glad.h
	include/Graphics.h
	include/Engine.h
	include/Scene.h
	include/khrplatform.h
	include/Light.h
	include/Light_Material.h
	include/Material.h
	include/Mesh.h
	include/MeshRenderer.h
	include/Model.h
	include/opengl.h
	include/Renderer.h
	include/Shader.h
	include/stb_image.h
	include/Texture.h
	include/Transform.h
	include/Utilities.h
	include/window.h
	include/WorldObject.h
)

set(${module}_sources
	src/Logger.cpp
	src/Framebuffer.cpp
	src/Camera.cpp
	src/Component.cpp
	src/Resources.cpp
	src/Cube_Material.cpp
	src/def_stb_image.cpp
	src/glad.c
	src/Graphics.cpp
	src/Engine.cpp
	src/Scene.cpp
	src/Light.cpp
	src/Light_Material.cpp
	src/Material.cpp
	src/Mesh.cpp
	src/MeshRenderer.cpp
	src/Model.cpp
	src/Renderer.cpp
	src/Shader.cpp
	src/Texture.cpp
	src/Transform.cpp
	src/Utilities.cpp
	src/window.cpp
	src/WorldObject.cpp
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

target_link_libraries(${module} LuaCpp Compute VoxelEngine)



