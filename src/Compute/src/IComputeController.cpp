#include "IComputeController.h"
#include "IComputeController_private.h"

using namespace DynamicCompute::Compute;

void IComputeController::Init(Platform platform, void* device, std::string program_dir)
{
	p_inst->Init(platform, device, program_dir);
}

IComputeProgram* IComputeController::AddProgram(IComputeProgram::ProgramInfo& info)
{
	return new IComputeProgram(p_inst->AddProgram(*((IComputeProgram_private::ProgramInfo*)&info)));
}

/*IComputeProgram* DynamicCompute::Compute::IComputeController::AddProgram(std::string name, std::string kernel, std::vector<uint8_t> data)
{
	return nullptr;
}*/

IComputeProgram* IComputeController::GetProgram(std::string name)
{
	return new IComputeProgram(p_inst->GetProgram(name));
}

IComputeBuffer* IComputeController::NewReadBuffer(size_t numElements, size_t stride, bool external)
{
	return new IComputeBuffer(p_inst->NewReadBuffer(numElements, stride, external));
}

IComputeBuffer* IComputeController::NewWriteBuffer(size_t numElements, size_t stride, bool external)
{
	return new IComputeBuffer(p_inst->NewWriteBuffer(numElements, stride, external));
}

IComputeBuffer* IComputeController::NewReadWriteBuffer(size_t numElements, size_t stride, bool external)
{
	return new IComputeBuffer(p_inst->NewReadWriteBuffer(numElements, stride, external));
}

void IComputeController::Dispose()
{
	p_inst->Dispose();
	p_inst = nullptr;
}
