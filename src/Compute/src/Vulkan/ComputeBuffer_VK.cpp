#include "ComputeBuffer_VK.h"

#include "ComputeEngine.h"

using namespace DynamicCompute::Compute::VK;

int ComputeBuffer_VK::SetData(void* data)
{
	return m_buffer->SetData(data);
}

int ComputeBuffer_VK::GetData(void* outData)
{
	return m_buffer->GetData(outData);
}

int DynamicCompute::Compute::VK::ComputeBuffer_VK::SetData(void* data, int size)
{
	return m_buffer->SetData(data, size);
}

int DynamicCompute::Compute::VK::ComputeBuffer_VK::GetData(void* outData, int size)
{
	return m_buffer->GetData(outData, size);
}

int DynamicCompute::Compute::VK::ComputeBuffer_VK::SetData(void* data, int DstStart, int size)
{
	return m_buffer->SetData(data, DstStart, size);
}

int DynamicCompute::Compute::VK::ComputeBuffer_VK::GetData(void* outData, int SrcStart, int size)
{
	return m_buffer->GetData(outData, SrcStart, size);
}

int DynamicCompute::Compute::VK::ComputeBuffer_VK::CopyTo(IComputeBuffer_private* other)
{
	ComputeBuffer_VK* other_vk = (ComputeBuffer_VK*)other;
	return 0;
}

int DynamicCompute::Compute::VK::ComputeBuffer_VK::CopyTo(IComputeBuffer_private* other, int size)
{
	ComputeBuffer_VK* other_vk = (ComputeBuffer_VK*)other;
	return 0;
}

int DynamicCompute::Compute::VK::ComputeBuffer_VK::CopyTo(IComputeBuffer_private* other, int srcStart, int dstStart, int size)
{
	ComputeBuffer_VK* other_vk = (ComputeBuffer_VK*)other;
	return 0;
}

size_t ComputeBuffer_VK::GetSize()
{
	return (size_t)m_buffer->GetSize();
}

void ComputeBuffer_VK::Dispose()
{
	m_buffer->Dispose();
}

void* ComputeBuffer_VK::Get_Native_Ptr()
{
	return m_buffer->Get_VK_Mem();
}


unsigned int ComputeBuffer_VK::External_Buffer()
{
	return m_buffer->External_GL_Buffer();
}

unsigned int ComputeBuffer_VK::External_Memory()
{
	return m_buffer->External_Memory();
}
