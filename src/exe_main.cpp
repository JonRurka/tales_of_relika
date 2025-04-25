#include <iostream>

#include "game_imp.h"
#include "Resources.h"
#include "Game_Resources.h"
#include "Utilities.h"

#include "dynamic_compute.h"
#include "shared_structures.h"
//#include "nvvk/gl_vk_vertex_buffer.h"
//#include "Compute/include/nvpro_core/nvvk/gl_vk_vertex_buffer.h"

#include <LuaCpp.hpp>

using namespace LuaCpp;
using namespace LuaCpp::Registry;
using namespace LuaCpp::Engine;
using namespace DynamicCompute::Compute;


int main()
{
	Logger::Set_Direct(true);

	//window::load_module();
	window w;
	w.Create_Window("test", 100, 100);

	std::vector<Vulkan_Device_Info> devices = ComputeInterface::GetSupportedDevices_Vulkan();
	Vulkan_Device_Info m_device = devices[1];
	//Logger::LogDebug(LOG_POS("InitializeComputePrograms"), "Using Compute Device: %s", m_device.Name);

	//printf("Creating programs...\n");
	//printf(m_shaderDir.c_str());

	ComputeInterface::ControllerInfo m_controllerInfo{};
	m_controllerInfo.device = &m_device;
	m_controllerInfo.SetProgramDir("");
	//m_controllerInfo.SetProgramDir("C:/Users/Jon/Source/cpp_libs/VoxelEngine/shaders/Vulkan/test");

	IComputeController* m_controller = ComputeInterface::GetComputeController(ComputeInterface::VULKAN, m_controllerInfo);
	m_controller->NewReadWriteBuffer(1, 1, true);

	//nvvk::gl_vk_buffer::poll_events();


	//Game game;
	//return game.Run();
}