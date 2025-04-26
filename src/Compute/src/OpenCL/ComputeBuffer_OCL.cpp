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
	return 0;
}

int ComputeBuffer_OCL::GetData(void* outData, int size)
{
	// TODO: Implement
	return 0;
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
	return 0;
}

unsigned int ComputeBuffer_OCL::External_Memory()
{
	return 0;
}
