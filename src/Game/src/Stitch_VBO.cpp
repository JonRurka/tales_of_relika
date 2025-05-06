#include "Stitch_VBO.h"

#include "Resources.h"
#include "Game_Resources.h"
#include "dynamic_compute.h"
#include "Logger.h"

#define STRIDE 8

void Stitch_VBO::Init(IVoxelBuilder_private* builder, int elements)
{
	v_builder = builder;
	m_controller = builder->Get_Compute_Controller();
	m_elements = elements;

	m_vertices = new glm::vec4[UINT16_MAX];
	m_normals = new glm::vec4[UINT16_MAX];
	m_triangles = new unsigned int[UINT16_MAX];

	std::string resource = Game_Resources::Shaders::Compute::CREATE_VBO;

	std::vector<char> shader_data_tmp = Resources::Get_Shader_bin(resource);
	std::vector<uint8_t> shader_data(shader_data_tmp.begin(), shader_data_tmp.end());

	//IComputeProgram::ProgramInfo program_info = IComputeProgram::ProgramInfo(m_name, IComputeProgram::FileType::Binary);
	IComputeProgram::ProgramInfo program_info = IComputeProgram::ProgramInfo(resource, IComputeProgram::FileType::Text_Data);
	program_info.SetData(shader_data);

	program_info.AddKernel(kernel_name);

	m_program = m_controller->AddProgram(program_info);
	m_program->Build();
	Logger::LogDebug(LOG_POS("Init"), "Build program");

	vertex_buffer = m_controller->NewReadWriteBuffer(elements, sizeof(float) * 4);
	normal_buffer = m_controller->NewReadWriteBuffer(elements, sizeof(float) * 4);
	Logger::LogDebug(LOG_POS("Init"), "Create Extern VBO");
	vbo_buffer = m_controller->NewReadWriteBuffer(elements, STRIDE * sizeof(float), true);

	IComputeProgram::BindIndex ind{};
	ind.GlobalIndex = 0;
	ind.ParameterIndex = 0;
	m_program->KernelSetBuffer(kernel_name, vbo_buffer, ind);

	ind = {};
	ind.GlobalIndex = 1;
	ind.ParameterIndex = 1;
	m_program->KernelSetBuffer(kernel_name, vertex_buffer, ind);

	ind = {};
	ind.GlobalIndex = 2;
	ind.ParameterIndex = 2;
	m_program->KernelSetBuffer(kernel_name, normal_buffer, ind);

	m_program->FinishBuild();
	Logger::LogDebug(LOG_POS("Init"), "Finish Build program");
}

void Stitch_VBO::Stitch(int elements)
{
	m_program->RunKernel(kernel_name, elements, 1, 1);

	Logger::LogDebug(LOG_POS("Stitch"), "Executed Kernel");
}

void Stitch_VBO::Process(Mesh* mesh, glm::ivec4 count, bool gpu_copy)
{
	if (gpu_copy) {

		v_builder->Extract(
			Input_Vertex_Buffer(),
			Input_Normal_Buffer(),
			nullptr,
			count
		);

		Stitch(count.x);
		Output_VBO_Buffer()->FlushExternal();

		mesh->Set_Vertex_Attributes(Get_Vertex_Attributes());
		mesh->Load(Output_VBO_Buffer());
	}
	else {

		v_builder->Extract(
			m_vertices,
			m_normals,
			m_triangles,
			count
		);

		std::vector<glm::vec4> verts(m_vertices, m_vertices + count.x);
		std::vector<unsigned int> tris(m_triangles, m_triangles + count.x);
		std::vector<glm::vec4> normals(m_normals, m_normals + count.x);

		mesh->Vertices(Utilities::vec4_to_vec3_arr(verts));
		mesh->Indices(tris);
		mesh->Normals(Utilities::vec4_to_vec3_arr(normals));
		mesh->Activate();
	}
}

int Stitch_VBO::Stride()
{
	return STRIDE;
}

Mesh::VertexAttributeList Stitch_VBO::Get_Vertex_Attributes()
{
	Mesh::VertexAttributeList res(STRIDE);
	res.add_attribute(4, 0);
	res.add_attribute(4, (4 * sizeof(float)));
	return res;
}
