#include "ComputeBuffer_OCL.h"

using namespace DynamicCompute::Compute::OCL;

int ComputeBuffer_OCL::SetData(void* data)
{
	return m_buffer->SetData(data);
}

int ComputeBuffer_OCL::GetData(void* outData)
{
	return m_buffer->GetData(outData);
}

int ComputeBuffer_OCL::SetData(void* data, int size)
{
	// TODO: Implement
	return m_buffer->SetData(data, size);
}

int ComputeBuffer_OCL::GetData(void* outData, int size)
{
	// TODO: Implement
	return m_buffer->GetData(outData, size);
}

int ComputeBuffer_OCL::SetData(void* data, int DstStart, int size)
{
	return m_buffer->SetData(data, DstStart, size);
}

int ComputeBuffer_OCL::GetData(void* outData, int SrcStart, int size)
{
	return m_buffer->GetData(outData, SrcStart, size);
}

int ComputeBuffer_OCL::CopyTo(IComputeBuffer_private* other)
{
	ComputeBuffer_OCL* other_vk = (ComputeBuffer_OCL*)other;
	return m_buffer->CopyTo(other_vk->GetBuffer());
}

int ComputeBuffer_OCL::CopyTo(IComputeBuffer_private* other, int size)
{
	ComputeBuffer_OCL* other_vk = (ComputeBuffer_OCL*)other;
	return m_buffer->CopyTo(other_vk->GetBuffer(), size);
}

int ComputeBuffer_OCL::CopyTo(IComputeBuffer_private* other, int srcStart, int dstStart, int size)
{
	ComputeBuffer_OCL* other_vk = (ComputeBuffer_OCL*)other;
	return m_buffer->CopyTo(other_vk->GetBuffer(), srcStart, dstStart, size);
}

void ComputeBuffer_OCL::FlushExternal()
{
	m_buffer->FlushExternal();
}

size_t ComputeBuffer_OCL::GetSize()
{
	return m_buffer->GetSize();
}

void ComputeBuffer_OCL::Dispose()
{
	m_buffer->Dispose();
}

void* ComputeBuffer_OCL::Get_Native_Ptr()
{
	return m_buffer->Get_CL_Mem();
}

unsigned int ComputeBuffer_OCL::External_Buffer()
{
	return m_buffer->External_GL_Buffer();
}

unsigned int ComputeBuffer_OCL::External_Memory()
{
	return m_buffer->External_Memory();
}
