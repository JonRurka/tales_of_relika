#pragma once

#include "shared_structures.h"
#include "IVoxelBuilder_private.h"

#include <queue>

namespace VoxelEngine {

	class ChunkProcessor_private {
	public:

		ChunkProcessor_private(ChunkProcessorSettings settings);

		void PushRequests(ChunkRequest** request, int numRequests);

		int Process();

		void Dispose();

	private:


		struct BatchRequest {
			ChunkRequest** requests;
			int numRequests;

			void Dispose() {
				delete[] requests;
				requests = nullptr;
			}
		};

		IVoxelBuilder_private* m_builder{ nullptr };

		ChunkProcessorSettings m_processor_settings{};

		std::queue<BatchRequest> m_request_batches;

	};

}