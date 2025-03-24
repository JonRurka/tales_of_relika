#include "ChunkProcessor.h"
#include "ChunkProcessor_private.h"

using namespace VoxelEngine;


VoxelEngine::ChunkProcessor::ChunkProcessor(ChunkProcessorSettings processor_settings)
{
	m_inst = new ChunkProcessor_private(processor_settings);
}

void VoxelEngine::ChunkProcessor::PushRequests(ChunkRequest** request, int numRequests)
{
	m_inst->PushRequests(request, numRequests);
}

int VoxelEngine::ChunkProcessor::Process()
{
	return m_inst->Process();
}

void VoxelEngine::ChunkProcessor::Dispose()
{
	m_inst->Dispose();
	delete m_inst;
	m_inst = nullptr;
}


