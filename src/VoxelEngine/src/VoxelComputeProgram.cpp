#include "VoxelComputeProgram.h"

#include "Resources.h"

using namespace DynamicCompute::Compute;
using namespace VoxelEngine;

VoxelComputeProgram::VoxelComputeProgram(IComputeController* controller, std::string name, IComputeProgram::FileType type)
{
	Initialize(controller, name, 16, type);
}

VoxelComputeProgram::VoxelComputeProgram(IComputeController* controller, std::string name, int workGroupSize, IComputeProgram::FileType type)
{
	Initialize(controller, name, workGroupSize, type);
}

void VoxelComputeProgram::AddBuffer(int bind, IComputeBuffer* buffer)
{
	if (finalized || !initialized)
		return;

	IComputeProgram::BindIndex ind{};
	ind.GlobalIndex = bind;
	ind.ParameterIndex = bind;
	m_program->KernelSetBuffer(kernel_name, buffer, ind);
}

void VoxelComputeProgram::Finalize()
{
	if (finalized || !initialized)
		return;

	m_program->FinishBuild();
	finalized = true;
}

void VoxelComputeProgram::Execute(int size_x, int size_y, int size_z)
{
	if (!finalized || !initialized)
		return;

	m_program->RunKernel(kernel_name, size_x, size_y, size_z);
}

void VoxelComputeProgram::Initialize(IComputeController* controller, std::string name, int workGroupSize, IComputeProgram::FileType type)
{
	m_controller = controller;
	m_name = name;

	switch (type) {
	case IComputeProgram::FileType::Binary_Data: // likely vulkan
		kernel_name = "main";
		break;
	case IComputeProgram::FileType::Text_Data: // likely OpenCL
		kernel_name = "main_cl";
		break;
	}


	std::vector<char> shader_data_tmp = Resources::Get_Shader_bin(name);
	std::vector<uint8_t> shader_data(shader_data_tmp.begin(), shader_data_tmp.end());

	//IComputeProgram::ProgramInfo program_info = IComputeProgram::ProgramInfo(m_name, IComputeProgram::FileType::Binary);
	//IComputeProgram::ProgramInfo program_info = IComputeProgram::ProgramInfo(m_name, IComputeProgram::FileType::Binary_Data);
	IComputeProgram::ProgramInfo program_info = IComputeProgram::ProgramInfo(m_name, type);
	program_info.SetData(shader_data);
	
	program_info.AddKernel(kernel_name);

	m_program = m_controller->AddProgram(program_info);
	m_program->Build();

	initialized = true;
}


