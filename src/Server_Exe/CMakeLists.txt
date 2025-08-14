project("tales_of_relika_server")



# Add X11 for screenshot support on linux
if(UNIX)
  find_package(X11 REQUIRED)
  LIST(APPEND PLATFORM_LIBRARIES ${X11_LIBRARIES})
endif()

# Make Windows compilation somewhat faster, and link with Dbghelp for
# stack trace information
if(WIN32)
  LIST(APPEND PLATFORM_LIBRARIES DbgHelp)
endif()


set(${PROJECT_NAME}_sources
    server_exe_main.cpp
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_sources})

target_link_libraries(${PROJECT_NAME} 
	${PLATFORM_LIBRARIES}
	${VULKAN_LIBS}
	ZLIB::ZLIB
	assimp::assimp 
	nlohmann_json::nlohmann_json 
	boost::boost
	glm::glm
	sol2::sol2
	Bullet::Bullet
	Jolt::Jolt
	inih::inih
	imgui::imgui
	#rmlui
	#rmlui_debugger
	#rmlui_shell
)
target_link_libraries(${PROJECT_NAME} Compute VoxelEngine Engine Game Server)
