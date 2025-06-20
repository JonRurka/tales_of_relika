set(module "Server")

include_directories(include)
include_directories(include/Network)

set(${module}_headers
	include/Network/AsyncServer.h
	include/Network/BufferUtils.h
	include/Network/Data.h
	include/Network/OpCodes.h
	include/Network/PlayerAuthenticator.h
	include/Network/SocketUser.h
	include/Network/TCP_Connection.h
	include/Network/TCP_Server.h
	include/Network/udp_connection.h
	include/Network/UDP_Server.h
	
	include/Network/NetClient.h

	include/CommandExecuter.h
	include/HashHelper.h
	include/IUser.h
	include/Linux_conio.h
	include/WorldController.h
	include/World.h
	include/Player.h
	include/Server_Main.h
	include/stdafx.h
	
)

set(${module}_sources
	src/Network/AsyncServer.cpp
	src/Network/AsyncServer_SocketUser.cpp
	src/Network/PlayerAuthenticator.cpp
	src/Network/TCP_Connection.cpp
	src/Network/TCP_Server.cpp
	src/Network/udp_connection.cpp
	src/Network/UDP_Server.cpp
	
	src/Network/NetClient.cpp
	
	src/CommandExecuter.cpp
	src/IUser.cpp
	src/WorldController.cpp
	src/World.cpp
	src/Linux_conio.cpp
	src/Player.cpp
	src/Server_Main.cpp
)

add_library(${module} STATIC ${${module}_headers} ${${module}_sources})

target_link_libraries(${module} 
	ZLIB::ZLIB
	assimp::assimp 
	nlohmann_json::nlohmann_json 
	glaze::glaze 
	boost::boost
	Vulkan::Vulkan
	glm::glm
	glfw
	sol2::sol2
	Bullet::Bullet
	OpenAL::OpenAL
	inih::inih
	#vulkan-headers::vulkan-headers
	#GPUOpen::VulkanMemoryAllocator
)

target_link_libraries(${module} Compute VoxelEngine Engine)





