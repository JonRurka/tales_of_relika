#pragma once

#include "shared_structures.h"
#include "IVoxelBuilder.h"

namespace VoxelEngine {

	class ChunkProcessor;

	class ChunkProcessor {
	public:

		ChunkProcessor(ChunkProcessorSettings settings);

		void PushRequests(ChunkRequest** request, int numRequests);

		int Process();

		void Dispose();

	private:

		ChunkProcessor_private* m_inst{ nullptr };

	};
	
}