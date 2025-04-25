#include "VoxelComputeProgram.h"

#include "Resources.h"

using namespace DynamicCompute::Compute;
using namespace VoxelEngine;

VoxelComputeProgram::VoxelComputeProgram(IComputeController* controller, std::string name) 
{
	Initialize(controller, name, 16);
}

VoxelComputeProgram::VoxelComputeProgram(IComputeController* controller, std::string name, int workGroupSize)
{
	Initialize(controller, name, workGroupSize);
}

void VoxelComputeProgram::AddBuffer(int bind, IComputeBuffer* buffer)
{
	if (finalized || !initialized)
		return;

	IComputeProgram::BindIndex ind{};
	ind.GlobalIndex = bind;
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

void VoxelComputeProgram::Initialize(IComputeController* controller, std::string name, int workGroupSize)
{
	m_controller = controller;
	m_name = name;



	std::vector<char> shader_data_tmp = Resources::Get_Shader_bin(name);
	std::vector<uint8_t> shader_data(shader_data_tmp.begin(), shader_data_tmp.end());

	//IComputeProgram::ProgramInfo program_info = IComputeProgram::ProgramInfo(m_name, IComputeProgram::FileType::Binary);
	IComputeProgram::ProgramInfo program_info = IComputeProgram::ProgramInfo(m_name, IComputeProgram::FileType::Raw);
	program_info.SetData(shader_data);
	
	program_info.AddKernel(kernel_name);

	m_program = m_controller->AddProgram(program_info);
	m_program->Build();

	initialized = true;
}


