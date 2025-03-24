#include "ChunkProcessor_private.h"
#include "SmoothVoxelBuilder.h"

using namespace VoxelEngine;

int Hash_Chunk(int x, int y, int z) {
	return x ^ y << 2 ^ z >> 2;
}

void crash_program() {
	int* tmp = nullptr;
	printf("%i", *tmp);
}

VoxelEngine::ChunkProcessor_private::ChunkProcessor_private(ChunkProcessorSettings processor_settings)
{
	ChunkSettings settings;
	ChunkGenerationOptions gen_options;
	ChunkRenderOptions render_options;

	m_processor_settings = processor_settings;

	settings.GetSettings()->setString("programDir", std::string(processor_settings.ProgramDirectory));
	settings.GetSettings()->setFloat("voxelsPerMeter", processor_settings.voxelsPerMeter);
	settings.GetSettings()->setInt("chunkMeterSizeX", processor_settings.chunkMeterSizeX);
	settings.GetSettings()->setInt("chunkMeterSizeY", processor_settings.chunkMeterSizeY);
	settings.GetSettings()->setInt("chunkMeterSizeZ", processor_settings.chunkMeterSizeZ);
	settings.GetSettings()->setInt("TotalBatchGroups", processor_settings.TotalBatchGroups);
	settings.GetSettings()->setInt("BatchesPerGroup", processor_settings.BatchesPerGroup);
	settings.GetSettings()->setInt("InvertTrianges", processor_settings.InvertTrianges ? 1 : 0);

	m_builder = new SmoothVoxelBuilder();

	m_builder->Init(&settings);

}

void VoxelEngine::ChunkProcessor_private::PushRequests(ChunkRequest** request, int numRequests)
{
	int b_size = m_processor_settings.TotalBatchGroups * m_processor_settings.BatchesPerGroup;

	BatchRequest req{};
	req.requests = new ChunkRequest*[b_size];
	memcpy((void*)req.requests, (void*)request, b_size * sizeof(ChunkRequest*));
	req.numRequests = numRequests;
	m_request_batches.push(req);

	/*for (int i = 0; i < b_size; i++) {
		printf("Added chunk at (%i, %i, %i).\n",
			req.requests[i]->X, req.requests[i]->Y, req.requests[i]->Z);
	}*/
}

int VoxelEngine::ChunkProcessor_private::Process()
{
	int total_chunks = m_processor_settings.BatchesPerGroup * m_processor_settings.TotalBatchGroups;

	while (!m_request_batches.empty()) {
		ChunkGenerationOptions gen_options{};
		ChunkRenderOptions render_options{};

		BatchRequest req = m_request_batches.front();
		m_request_batches.pop();

		for (int i = 0; i < total_chunks; i++) {
			glm::ivec4 chunk_loc = glm::ivec4(-1, -1, -1, -1);
			if (i < req.numRequests) {
				ChunkRequest* c_req = req.requests[i];

				chunk_loc = glm::ivec4(c_req->X, c_req->Y, c_req->Z, 0);
			}
			gen_options.locations.push_back(chunk_loc);
			render_options.locations.push_back(chunk_loc);

			//printf("Added (%i, %i, %i) to options.\n",
			//	chunk_loc.x, chunk_loc.y, chunk_loc.z);
		}

		glm::dvec4 gen_times = m_builder->Generate(&gen_options);

		glm::dvec4 render_times = m_builder->Render(&render_options);

		std::vector<glm::ivec4> counts = m_builder->GetSize();

		for (int i = 0; i < req.numRequests; i++) {
			glm::ivec4 chnk_count = counts[i];

			req.requests[i]->m_numVertex = chnk_count.x;
			req.requests[i]->m_numTriangles = chnk_count.x;

			req.requests[i]->m_vertices = new OutVertex3D[chnk_count.x];
			req.requests[i]->m_normals = new OutVertex3D[chnk_count.x];
			req.requests[i]->m_triangles = new int[chnk_count.x];

			m_builder->Extract(
				(glm::vec4*)req.requests[i]->m_vertices,
				(glm::vec4*)req.requests[i]->m_normals,
				req.requests[i]->m_triangles,
				counts[i]
			);

			bool is_all_zero = true;
			for (int j = 0; j < req.requests[i]->m_numVertex; j++) {
				if (req.requests[i]->m_vertices[j].X != 0 ||
					req.requests[i]->m_vertices[j].Y != 0 ||
					req.requests[i]->m_vertices[j].Z != 0) {

					is_all_zero = false;
					break;
				}
			}

			if (is_all_zero) {
				printf("They were all zero?\n");
				//crash_program();
			}


			req.requests[i]->m_complete = true;

		}

		req.Dispose();
	}

	return 0;
}

void VoxelEngine::ChunkProcessor_private::Dispose()
{
	m_builder->Dispose();
}
