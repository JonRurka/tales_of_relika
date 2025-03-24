#include "IComputeProgram.h"
#include "IComputeBuffer.h"
#include "IComputeProgram_private.h"

using namespace DynamicCompute::Compute;

void DynamicCompute::Compute::IComputeProgram::AddIncludeDirectory(std::string directory)
{
	p_inst->AddIncludeDirectory(directory);
}

void DynamicCompute::Compute::IComputeProgram::AddDefine(std::string name, std::string value)
{
	p_inst->AddDefine(name, value);
}

int DynamicCompute::Compute::IComputeProgram::Build()
{
	return p_inst->Build();
}

int DynamicCompute::Compute::IComputeProgram::FinishBuild()
{
	return p_inst->FinishBuild();
}

int DynamicCompute::Compute::IComputeProgram::GetKernelID(std::string name)
{
	return p_inst->GetKernelID(name);
}

void DynamicCompute::Compute::IComputeProgram::KernelSetWorkGroupSize(std::string k_name, int size)
{
	p_inst->KernelSetWorkGroupSize(k_name, size);
}

int DynamicCompute::Compute::IComputeProgram::KernelSetBuffer(std::string k_name, IComputeBuffer* buffer, BindIndex arg)
{
	return p_inst->KernelSetBuffer(k_name, buffer->p_inst, *((IComputeProgram_private::BindIndex*)&arg));
}

int DynamicCompute::Compute::IComputeProgram::RunKernel(std::string k_name, int size_x, int size_y, int size_z)
{
	return p_inst->RunKernel(k_name, size_x, size_y, size_z);
}

int DynamicCompute::Compute::IComputeProgram::RunKernel(int kernel_id, int size_x, int size_y, int size_z)
{
	return p_inst->RunKernel(kernel_id, size_x, size_y, size_z);
}

void* DynamicCompute::Compute::IComputeProgram::GetKernelFunction(int kernel_id)
{
	return p_inst->GetKernelFunction(kernel_id);
}

IComputeProgram::ProgramBuildState DynamicCompute::Compute::IComputeProgram::GetState()
{
	IComputeProgram_private::ProgramBuildState val = p_inst->GetState();
	return *((IComputeProgram::ProgramBuildState*)&val);
}

int DynamicCompute::Compute::IComputeProgram::GetBuildResultCode()
{
	return p_inst->GetBuildResultCode();
}

std::string DynamicCompute::Compute::IComputeProgram::GetBuildErrorMessage()
{
	return p_inst->GetBuildErrorMessage();
}

std::string DynamicCompute::Compute::IComputeProgram::GetProgramName()
{
	return p_inst->GetProgramName();
}

void DynamicCompute::Compute::IComputeProgram::Dispose()
{
	p_inst->Dispose();
	p_inst = nullptr;
}
