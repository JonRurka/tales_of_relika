#include "ComputeBuffer_OCL.h"

using namespace DynamicCompute::Compute::OCL;

int DynamicCompute::Compute::OCL::ComputeBuffer_OCL::SetData(void* data)
{
	return m_buffer->SetData(data);
}

int DynamicCompute::Compute::OCL::ComputeBuffer_OCL::GetData(void* outData)
{
	return m_buffer->GetData(outData);
}

int DynamicCompute::Compute::OCL::ComputeBuffer_OCL::SetData(void* data, int size)
{
	// TODO: Implement
	return 0;
}

int DynamicCompute::Compute::OCL::ComputeBuffer_OCL::GetData(void* outData, int size)
{
	// TODO: Implement
	return 0;
}

size_t DynamicCompute::Compute::OCL::ComputeBuffer_OCL::GetSize()
{
	return m_buffer->GetSize();
}

void DynamicCompute::Compute::OCL::ComputeBuffer_OCL::Dispose()
{
	m_buffer->Dispose();
}

void* DynamicCompute::Compute::OCL::ComputeBuffer_OCL::Get_Native_Ptr()
{
	return m_buffer->Get_CL_Mem();
}
