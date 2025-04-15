#pragma once

//#include "shared_structures.h"
#include "shared_structures.h"

namespace VoxelEngine {

	class IVoxelBuilder_private;

	class IVoxelBuilder {
	public:

		enum class VoxelBuilderType {
			SmoothVoxelBuilder,
		};

		void Init(ChunkSettings* settings);

		//virtual void SetBlockTypes(BlockType* blockTypeList, Rect* AtlasUvs) = 0;

		glm::dvec4 Render(ChunkRenderOptions* options);

		glm::dvec4 Generate(ChunkGenerationOptions* options);

		std::vector<glm::ivec4> GetSize();

		void Extract(glm::vec4* out_vertex, glm::vec4* out_normal, unsigned int* out_trianges, glm::ivec4 counts);

		void ReleaseHeightmap(int x, int z);

		void SetHeightmapsEnabled(bool enabled);

		//virtual void SetBlock(int x, int y, int z, Block block) = 0;

		//virtual Block GetBlock(int x, int y, int z) = 0;

		//virtual void SetSurroundingChunks() = 0;

		virtual void Dispose();

		static IVoxelBuilder* GetVoxelBuilder(VoxelBuilderType type);

	private:

		IVoxelBuilder(IVoxelBuilder_private* inst) { m_inst = inst; }
		IVoxelBuilder_private* m_inst = nullptr;
	};

	

}