#include "IComputeBuffer.h"
#include "IComputeBuffer_private.h"

using namespace DynamicCompute;
using namespace DynamicCompute::Compute;

int IComputeBuffer::SetData(void* data)
{
	return p_inst->SetData(data);
}

int IComputeBuffer::GetData(void* outData)
{
	return p_inst->GetData(outData);
}

int IComputeBuffer::SetData(void* data, int size)
{
	return p_inst->SetData(data, size);
}

int IComputeBuffer::GetData(void* outData, int size)
{
	return p_inst->GetData(outData, size);
}

int IComputeBuffer::SetData(void* data, int DstStart, int size)
{
	return p_inst->SetData(data, DstStart, size);
}

int IComputeBuffer::GetData(void* outData, int SrcStart, int size)
{
	return p_inst->GetData(outData, SrcStart, size);
}

int IComputeBuffer::CopyTo(IComputeBuffer* other)
{
	return p_inst->CopyTo(other->p_inst);
}

int IComputeBuffer::CopyTo(IComputeBuffer* other, int size)
{
	return p_inst->CopyTo(other->p_inst, size);
}

int IComputeBuffer::CopyTo(IComputeBuffer* other, int srcStart, int dstStart, int size)
{
	return p_inst->CopyTo(other->p_inst, srcStart, dstStart, size);
}

void IComputeBuffer::FlushExternal()
{
	p_inst->FlushExternal();
}

size_t IComputeBuffer::GetSize()
{
	return p_inst->GetSize();
}

void IComputeBuffer::Dispose()
{
	p_inst->Dispose();
	p_inst = nullptr;
}

void* IComputeBuffer::Get_Native_Ptr()
{
	return p_inst->Get_Native_Ptr();
}

unsigned int IComputeBuffer::External_Buffer()
{
	return p_inst->External_Buffer();
}

unsigned int IComputeBuffer::External_Memory()
{
	return p_inst->External_Memory();
}

