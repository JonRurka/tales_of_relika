#pragma once

#include "game_engine.h"
#include "shared_structures.h"
#include "IVoxelBuilder.h"
#include "SmoothVoxelBuilder.h"

#include <unordered_map>

namespace DynamicCompute {
	namespace Compute {
		class IComputeBuffer;
		class IComputeController;
		class IComputeProgram;
	}
}

using namespace DynamicCompute::Compute;
using namespace VoxelEngine;

class Stitch_VBO {
public:

	void Init(IVoxelBuilder_private* builder, int elements);
	void Stitch(int elements);
	void Process(Mesh* mesh, glm::ivec4 count, bool gpu_copy);
	void Reset();

	IComputeBuffer* Input_Vertex_Buffer() { return vertex_buffer; }
	IComputeBuffer* Input_Normal_Buffer() { return normal_buffer; }
	IComputeBuffer* Output_VBO_Buffer() { return vbo_buffer; }

	static int Stride();

	static Mesh::VertexAttributeList Get_Vertex_Attributes();

private:
	std::string kernel_name = "main_cl";
	int m_elements{ 0 };
	IComputeController* m_controller{nullptr};
	IComputeProgram* m_program{ nullptr };
	IVoxelBuilder_private* v_builder{ nullptr };

	const int Max_Verts = (int)Utilities::Vertex_Limit_Mode::Chunk_Max;

	glm::vec4* m_vertices{nullptr};
	glm::vec4* m_normals{ nullptr };
	unsigned int* m_triangles{ nullptr };

	IComputeBuffer* vertex_buffer{ nullptr };
	IComputeBuffer* normal_buffer{ nullptr };
	IComputeBuffer* vbo_buffer{ nullptr };

	OpenCL_Device_Info m_device_cl;
	IComputeController* create_controller();

	glm::dvec4 times{ glm::dvec4(0.0) };

	void compute_triangles();

	std::unordered_map<int, std::vector<glm::vec4>> debug_vert_data;

	inline static const std::string LOG_LOC{ "STITCH_VBO" };
};