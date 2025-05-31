#pragma once

#include "shared_structures.h"
#include "dynamic_compute.h"


using namespace DynamicCompute::Compute;

namespace VoxelEngine {

	
	struct BlockType;
	struct Rect;

	class IVoxelBuilder_private {
	public:

		virtual void Init(ChunkSettings* settings) = 0;

		//virtual void SetBlockTypes(BlockType* blockTypeList, Rect* AtlasUvs) = 0;

		virtual glm::dvec4 Render(ChunkRenderOptions* options) = 0;

		virtual glm::dvec4 Generate(ChunkGenerationOptions* options) = 0;

		virtual std::vector<glm::ivec4> GetSize() = 0;

		virtual void Extract(IComputeBuffer* out_vertex, IComputeBuffer* out_normal, IComputeBuffer* out_mat, IComputeBuffer* out_trianges, glm::ivec4 counts) = 0;

		virtual void Extract(glm::vec4* out_vertex, glm::vec4* out_normal, glm::vec4* out_mat, unsigned int* out_trianges, glm::ivec4 counts) = 0;

		virtual void ReleaseHeightmap(int x, int z) = 0;

		virtual void SetHeightmapsEnabled(bool enabled) = 0;

		virtual IComputeBuffer* Get_Tranfer_Buffer(int size, int stride, bool external) = 0;

		virtual IComputeController* Get_Compute_Controller() = 0;

		//virtual void SetBlock(int x, int y, int z, Block block) = 0;

		//virtual Block GetBlock(int x, int y, int z) = 0;

		//virtual void SetSurroundingChunks() = 0;

		virtual void Dispose() = 0;
	};

}