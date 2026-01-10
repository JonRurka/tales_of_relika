#pragma once

#include "game_engine.h"
#include "shared_structures.h"
#include "IVoxelBuilder.h"
#include "SmoothVoxelBuilder.h"

#include <unordered_map>
#include <memory>

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
	typedef std::shared_ptr<Stitch_VBO> Shared;
	typedef std::weak_ptr<Stitch_VBO> Weak;

	void Init(IVoxelBuilder_private::Shared builder, int elements);
	void Stitch(int elements);
	void Process(Mesh& mesh, glm::ivec4 count, bool gpu_copy, bool apply_mesh = true);
	void Reset();

	IComputeBuffer* Input_Vertex_Buffer() { return vertex_buffer; }
	IComputeBuffer* Input_Normal_Buffer() { return normal_buffer; }
	IComputeBuffer* Input_Mat_Buffer() { return mat_buffer; }
	IComputeBuffer* Output_VBO_Buffer() { return vbo_buffer; }

	std::vector<unsigned int> Triangle_Data() { return m_triangles; }

	static int Byte_Stride();
	static int Float_Stride();

	static Mesh::VertexAttributeList Get_Vertex_Attributes();

private:
	std::string kernel_name = "main_cl";
	int m_elements{ 0 };
	IComputeController* m_controller{nullptr};
	IComputeProgram* m_program{ nullptr };
	IVoxelBuilder_private::Shared v_builder;

	const int Max_Verts = (int)Utilities::Vertex_Limit_Mode::Chunk_Max;

	Mesh::VertexAttributeList m_attribute_list{};

	glm::vec4* m_vertices{nullptr};
	glm::vec4* m_normals{ nullptr };
	glm::vec4* m_mats{ nullptr };
	std::vector<unsigned int> m_triangles;
	float* m_raw_vert_data{nullptr};

	IComputeBuffer* vertex_buffer{ nullptr };
	IComputeBuffer* normal_buffer{ nullptr };
	IComputeBuffer* mat_buffer{ nullptr };
	IComputeBuffer* vbo_buffer{ nullptr };

	OpenCL_Device_Info m_device_cl{};
	IComputeController* create_controller();

	glm::dvec4 times{ glm::dvec4(0.0) };

	void compute_triangles();

	std::unordered_map<int, std::vector<glm::vec4>> debug_vert_data;

	inline static const std::string LOG_LOC{ "STITCH_VBO" };
};