#include "IComputeProgram.h"
#include "IComputeBuffer.h"
#include "IComputeProgram_private.h"

using namespace DynamicCompute::Compute;

void IComputeProgram::AddIncludeDirectory(std::string directory)
{
	p_inst->AddIncludeDirectory(directory);
}

void IComputeProgram::AddDefine(std::string name, std::string value)
{
	p_inst->AddDefine(name, value);
}

int IComputeProgram::Build()
{
	return p_inst->Build();
}

int IComputeProgram::FinishBuild()
{
	return p_inst->FinishBuild();
}

int IComputeProgram::GetKernelID(std::string name)
{
	return p_inst->GetKernelID(name);
}

void IComputeProgram::KernelSetWorkGroupSize(std::string k_name, glm::uvec3 size)
{
	p_inst->KernelSetWorkGroupSize(k_name, size);
}

int IComputeProgram::KernelSetBuffer(std::string k_name, IComputeBuffer* buffer, BindIndex arg)
{
	return p_inst->KernelSetBuffer(k_name, buffer->p_inst, *((IComputeProgram_private::BindIndex*)&arg));
}

int IComputeProgram::RunKernel(std::string k_name, int size_x, int size_y, int size_z)
{
	return p_inst->RunKernel(k_name, size_x, size_y, size_z);
}

int IComputeProgram::RunKernel(int kernel_id, int size_x, int size_y, int size_z)
{
	return p_inst->RunKernel(kernel_id, size_x, size_y, size_z);
}

int IComputeProgram::RunKernel(std::string k_name, int num, int size_x, int size_y, int size_z)
{
	return p_inst->RunKernel(k_name, num, size_x, size_y, size_z);
}

int IComputeProgram::RunKernel(int kernel_id, int num, int size_x, int size_y, int size_z)
{
	return p_inst->RunKernel(kernel_id, num, size_x, size_y, size_z);
}

void* IComputeProgram::GetKernelFunction(int kernel_id)
{
	return p_inst->GetKernelFunction(kernel_id);
}

IComputeProgram::ProgramBuildState IComputeProgram::GetState()
{
	IComputeProgram_private::ProgramBuildState val = p_inst->GetState();
	return *((IComputeProgram::ProgramBuildState*)&val);
}

int IComputeProgram::GetBuildResultCode()
{
	return p_inst->GetBuildResultCode();
}

std::string IComputeProgram::GetBuildErrorMessage()
{
	return p_inst->GetBuildErrorMessage();
}

std::string IComputeProgram::GetProgramName()
{
	return p_inst->GetProgramName();
}

void IComputeProgram::Dispose()
{
	p_inst->Dispose();
	p_inst = nullptr;
}
