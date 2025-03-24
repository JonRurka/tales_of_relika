#include "IVoxelBuilder.h"
#include "IVoxelBuilder_private.h"
#include "SmoothVoxelBuilder.h"

using namespace VoxelEngine;

void VoxelEngine::IVoxelBuilder::Init(ChunkSettings* settings)
{
	m_inst->Init(settings);
}

glm::dvec4 VoxelEngine::IVoxelBuilder::Render(ChunkRenderOptions* options)
{
	return m_inst->Render(options);
}

glm::dvec4 VoxelEngine::IVoxelBuilder::Generate(ChunkGenerationOptions* options)
{
	return m_inst->Generate(options);
}

std::vector<glm::ivec4> VoxelEngine::IVoxelBuilder::GetSize()
{
	return m_inst->GetSize();
}

void VoxelEngine::IVoxelBuilder::Extract(glm::vec4* out_vertex, glm::vec4* out_normal, int* out_trianges, glm::ivec4 counts)
{
	return m_inst->Extract(out_vertex, out_normal, out_trianges, counts);
}

void VoxelEngine::IVoxelBuilder::ReleaseHeightmap(int x, int z)
{
	m_inst->ReleaseHeightmap(x, z);
}

void VoxelEngine::IVoxelBuilder::SetHeightmapsEnabled(bool enabled)
{
	m_inst->SetHeightmapsEnabled(enabled);
}

void VoxelEngine::IVoxelBuilder::Dispose()
{
	m_inst->Dispose();
	delete m_inst;
	m_inst = nullptr;
}

IVoxelBuilder* VoxelEngine::IVoxelBuilder::GetVoxelBuilder(VoxelBuilderType type)
{
	IVoxelBuilder* result = nullptr;

	switch (type) {
	case VoxelBuilderType::SmoothVoxelBuilder:
		result = new IVoxelBuilder(new SmoothVoxelBuilder());
		break;
	}

	return result;
}
