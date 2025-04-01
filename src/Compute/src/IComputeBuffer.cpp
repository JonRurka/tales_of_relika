#include "IComputeBuffer.h"
#include "IComputeBuffer_private.h"

int DynamicCompute::Compute::IComputeBuffer::SetData(void* data)
{
	return p_inst->SetData(data);
}

int DynamicCompute::Compute::IComputeBuffer::GetData(void* outData)
{
	return p_inst->GetData(outData);
}

int DynamicCompute::Compute::IComputeBuffer::SetData(void* data, int size)
{
	return p_inst->SetData(data, size);
}

int DynamicCompute::Compute::IComputeBuffer::GetData(void* outData, int size)
{
	return p_inst->GetData(outData, size);
}

int DynamicCompute::Compute::IComputeBuffer::SetData(void* data, int DstStart, int size)
{
	return p_inst->SetData(data, DstStart, size);
}

int DynamicCompute::Compute::IComputeBuffer::GetData(void* outData, int SrcStart, int size)
{
	return p_inst->GetData(outData, SrcStart, size);
}

int DynamicCompute::Compute::IComputeBuffer::CopyTo(IComputeBuffer* other)
{
	return p_inst->CopyTo(other->p_inst);
}

int DynamicCompute::Compute::IComputeBuffer::CopyTo(IComputeBuffer* other, int size)
{
	return p_inst->CopyTo(other->p_inst, size);
}

int DynamicCompute::Compute::IComputeBuffer::CopyTo(IComputeBuffer* other, int srcStart, int dstStart, int size)
{
	return p_inst->CopyTo(other->p_inst, srcStart, dstStart, size);
}

size_t DynamicCompute::Compute::IComputeBuffer::GetSize()
{
	return p_inst->GetSize();
}

void DynamicCompute::Compute::IComputeBuffer::Dispose()
{
	p_inst->Dispose();
	p_inst = nullptr;
}

void* DynamicCompute::Compute::IComputeBuffer::Get_Native_Ptr()
{
	return p_inst->Get_Native_Ptr();
}

unsigned int DynamicCompute::Compute::IComputeBuffer::External_Buffer()
{
	return p_inst->External_Buffer();
}

unsigned int DynamicCompute::Compute::IComputeBuffer::External_Memory()
{
	return p_inst->External_Memory();
}

