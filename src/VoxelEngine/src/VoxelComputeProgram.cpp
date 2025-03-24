#include "VoxelComputeProgram.h"

using namespace DynamicCompute::Compute;

VoxelEngine::VoxelComputeProgram::VoxelComputeProgram(IComputeController* controller, std::string name) 
{
	Initialize(controller, name, 16);
}

VoxelEngine::VoxelComputeProgram::VoxelComputeProgram(IComputeController* controller, std::string name, int workGroupSize)
{
	Initialize(controller, name, workGroupSize);
}

void VoxelEngine::VoxelComputeProgram::AddBuffer(int bind, IComputeBuffer* buffer)
{
	if (finalized || !initialized)
		return;

	IComputeProgram::BindIndex ind{};
	ind.GlobalIndex = bind;
	m_program->KernelSetBuffer(kernel_name, buffer, ind);
}

void VoxelEngine::VoxelComputeProgram::Finalize()
{
	if (finalized || !initialized)
		return;

	m_program->FinishBuild();
	finalized = true;
}

void VoxelEngine::VoxelComputeProgram::Execute(int size_x, int size_y, int size_z)
{
	if (!finalized || !initialized)
		return;

	m_program->RunKernel(kernel_name, size_x, size_y, size_z);
}

void VoxelEngine::VoxelComputeProgram::Initialize(IComputeController* controller, std::string name, int workGroupSize)
{
	m_controller = controller;
	m_name = name;

	IComputeProgram::ProgramInfo program_info = IComputeProgram::ProgramInfo(m_name, IComputeProgram::FileType::Binary);
	program_info.AddKernel(kernel_name);

	m_program = m_controller->AddProgram(program_info);
	m_program->Build();

	initialized = true;
}


