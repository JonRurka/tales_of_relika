#include <iostream>

#include "game_imp.h"
#include "Resources.h"
#include "Game_Resources.h"
#include "Utilities.h"

#include "dynamic_compute.h"
#include "shared_structures.h"
#include "IVoxelBuilder.h"
#include "SmoothVoxelBuilder.h"
#include "Stitch_VBO.h"
//#include "nvvk/gl_vk_vertex_buffer.h"
//#include "Compute/include/nvpro_core/nvvk/gl_vk_vertex_buffer.h"

//#include <LuaCpp.hpp>

//using namespace LuaCpp;
//using namespace LuaCpp::Registry;
//using namespace LuaCpp::Engine;
using namespace DynamicCompute::Compute;
using namespace VoxelEngine;

#define STRIDE 8

int main()
{
	Logger::Set_Direct(true);

	/*window w;
	w.Create_Window("test", 100, 100);

	Resources* res = new Resources();
	ChunkSettings settings;
	ChunkGenerationOptions gen_options;
	ChunkRenderOptions render_options;

	settings.GetSettings()->setString("programDir", std::string("C:/Users/jrurka/Source/repos/game_project/resources/shaders/compute/voxelEngine/Bin"));
	settings.GetSettings()->setFloat("voxelsPerMeter", 1);
	settings.GetSettings()->setInt("chunkMeterSizeX", 8);
	settings.GetSettings()->setInt("chunkMeterSizeY", 8);
	settings.GetSettings()->setInt("chunkMeterSizeZ", 8);
	settings.GetSettings()->setInt("TotalBatchGroups", 1);
	settings.GetSettings()->setInt("BatchesPerGroup", 1);
	settings.GetSettings()->setInt("InvertTrianges", false);
	IVoxelBuilder_private* m_builder = new SmoothVoxelBuilder();

	m_builder->Init(&settings);

	Stitch_VBO* vbo_stitch = new Stitch_VBO();
	vbo_stitch->Init(m_builder, INT16_MAX);

	glm::fvec4* test_verts = new glm::fvec4[INT16_MAX];
	glm::fvec4* test_norms = new glm::fvec4[INT16_MAX];

	vbo_stitch->Input_Vertex_Buffer()->SetData(test_verts);
	vbo_stitch->Input_Normal_Buffer()->SetData(test_norms);
	vbo_stitch->Stitch(INT16_MAX);
	vbo_stitch->Output_VBO_Buffer()->FlushExternal();*/

	/*window w;
	w.Create_Window("test", 100, 100);

	IComputeProgram::FileType type = IComputeProgram::FileType::Text_Data;

	OpenCL_Device_Info m_device_cl = Utilities::Get_Recommended_Device();
	Logger::LogDebug("MAIN", "Using OpenCL Compute Device: %s", m_device_cl.name);

	ComputeInterface::ControllerInfo m_controllerInfo{};
	m_controllerInfo.device = &m_device_cl;
	m_controllerInfo.platform = m_device_cl.platform;
	m_controllerInfo.SetProgramDir("");

	IComputeController* m_controller = ComputeInterface::GetComputeController(ComputeInterface::OpenCL, m_controllerInfo);

	IComputeBuffer* vbo_buffer = m_controller->NewReadWriteBuffer(UINT16_MAX, STRIDE * sizeof(float), true);

	float* data = new float[UINT16_MAX * STRIDE];
	vbo_buffer->SetData(data);

	vbo_buffer->FlushExternal();*/









	//window::load_module();
	//window w;
	//w.Create_Window("test", 100, 100);

	//std::vector<Vulkan_Device_Info> devices = ComputeInterface::GetSupportedDevices_Vulkan();
	//Vulkan_Device_Info m_device = devices[1];
	//Logger::LogDebug(LOG_POS("InitializeComputePrograms"), "Using Compute Device: %s", m_device.Name);
	
	//std::vector<OpenCL_Device_Info> devices = ComputeInterface::GetSupportedDevices_OpenCL();

	//printf("Creating programs...\n");
	//printf(m_shaderDir.c_str());

	
	
	//nvvk::gl_vk_buffer::poll_events();

	Game game;
	return game.Run();
}