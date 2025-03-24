#include "IComputeController.h"
#include "IComputeController_private.h"

using namespace DynamicCompute::Compute;

void DynamicCompute::Compute::IComputeController::Init(Platform platform, void* device, std::string program_dir)
{
	p_inst->Init(platform, device, program_dir);
}

IComputeProgram* DynamicCompute::Compute::IComputeController::AddProgram(IComputeProgram::ProgramInfo& info)
{
	return new IComputeProgram(p_inst->AddProgram(*((IComputeProgram_private::ProgramInfo*)&info)));
}

/*IComputeProgram* DynamicCompute::Compute::IComputeController::AddProgram(std::string name, std::string kernel, std::vector<uint8_t> data)
{
	return nullptr;
}*/

IComputeProgram* DynamicCompute::Compute::IComputeController::GetProgram(std::string name)
{
	return new IComputeProgram(p_inst->GetProgram(name));
}

IComputeBuffer* DynamicCompute::Compute::IComputeController::NewReadBuffer(size_t numElements, size_t stride)
{
	return new IComputeBuffer(p_inst->NewReadBuffer(numElements, stride));
}

IComputeBuffer* DynamicCompute::Compute::IComputeController::NewWriteBuffer(size_t numElements, size_t stride)
{
	return new IComputeBuffer(p_inst->NewWriteBuffer(numElements, stride));
}

IComputeBuffer* DynamicCompute::Compute::IComputeController::NewReadWriteBuffer(size_t numElements, size_t stride)
{
	return new IComputeBuffer(p_inst->NewReadWriteBuffer(numElements, stride));
}

void DynamicCompute::Compute::IComputeController::Dispose()
{
	p_inst->Dispose();
	p_inst = nullptr;
}
