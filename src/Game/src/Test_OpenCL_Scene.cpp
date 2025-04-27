#include "Test_OpenCL_Scene.h"

#include "Game_Resources.h"

void Test_OpenCL_Scene::Init()
{
	std::vector<OpenCL_Device_Info> devices;
	std::vector<Platform> platforms = ComputeInterface::GetSupportedPlatforms_OpenCL();
	//printf("Reported Platforms: %i\n", (int)platforms.size());
	for (const auto& plt : platforms) {
		//printf("Platform Name: %s (%llX)\n", plt.name, (long long)plt.platform);
		std::vector<OpenCL_Device_Info> plt_devices = ComputeInterface::GetSupportedDevices_OpenCL(plt);
		for (const auto& device : plt_devices) {
			devices.push_back(device);
			//printf("\tDevice: %s, GPU: %i, CPU: %i, Comp Units: %i\n", device.name, (int)device.is_type_GPU, (int)device.is_type_CPU, device.num_compute_units);
		}
	}
	//printf("\n");

	OpenCL_Device_Info picked_device;
	picked_device.num_compute_units = 0;
	int max_comp = 0;
	for (const auto& elem : devices) {
		if (elem.num_compute_units > picked_device.num_compute_units) {
			picked_device = elem;
		}
	}

	ComputeInterface::ControllerInfo m_controllerInfo{};
	m_controllerInfo.device = &picked_device;
	m_controllerInfo.platform = picked_device.platform;
	m_controllerInfo.SetProgramDir("");
	//m_controllerInfo.SetProgramDir("C:/Users/Jon/Source/cpp_libs/VoxelEngine/shaders/Vulkan/test");

	IComputeController* m_controller = ComputeInterface::GetComputeController(ComputeInterface::OpenCL, m_controllerInfo);
	m_controller->NewReadWriteBuffer(1, 1, true);
	//printf("Selected Device: %s, GPU: %i, CPU: %i, Comp Units: %i\n", picked_device.name, (int)picked_device.is_type_GPU, (int)picked_device.is_type_CPU, picked_device.num_compute_units);

	std::string resource = Game_Resources::Shaders::Compute::TEST_OPENCL_SPIRV;

	std::vector<char> shader_data_tmp = Resources::Get_Shader_bin(resource);
	std::vector<uint8_t> shader_data(shader_data_tmp.begin(), shader_data_tmp.end());

	//IComputeProgram::ProgramInfo program_info = IComputeProgram::ProgramInfo(resource, IComputeProgram::FileType::Raw);
	//program_info.SetData(shader_data);

	IComputeProgram::ProgramInfo program_info = IComputeProgram::ProgramInfo(resource, IComputeProgram::FileType::Text_File);
	program_info.SetData(shader_data);

	int elements = 16;
	std::string kernel_name = "main";

	program_info.AddKernel(kernel_name);

	IComputeProgram* m_program = m_controller->AddProgram(program_info);
	m_program->Build();
	Logger::LogDebug(LOG_POS("Init"), "Build program");

	IComputeBuffer* in_buffer = m_controller->NewReadWriteBuffer(elements, sizeof(float) * 4);
	IComputeBuffer* out_buffer = m_controller->NewReadWriteBuffer(elements, sizeof(float) * 4);

	IComputeProgram::BindIndex ind{};
	ind.GlobalIndex = 0;
	m_program->KernelSetBuffer(kernel_name, in_buffer, ind);

	ind = {};
	ind.GlobalIndex = 1;
	m_program->KernelSetBuffer(kernel_name, out_buffer, ind);


	m_program->FinishBuild();
	Logger::LogDebug(LOG_POS("Init"), "Finish Build program");
}

void Test_OpenCL_Scene::Update(float dt)
{
}
