#include "Stitch_VBO.h"

#include "Resources.h"
#include "Game_Resources.h"
#include "dynamic_compute.h"
#include "Logger.h"

#define STRIDE 8

void Stitch_VBO::Init(IComputeController* controller, int elements)
{
	m_controller = controller;
	m_elements = elements;

	std::string resource = Game_Resources::Shaders::Compute::CREATE_VBO;

	std::vector<char> shader_data_tmp = Resources::Get_Shader_bin(resource);
	std::vector<uint8_t> shader_data(shader_data_tmp.begin(), shader_data_tmp.end());

	//IComputeProgram::ProgramInfo program_info = IComputeProgram::ProgramInfo(m_name, IComputeProgram::FileType::Binary);
	IComputeProgram::ProgramInfo program_info = IComputeProgram::ProgramInfo(resource, IComputeProgram::FileType::Raw);
	program_info.SetData(shader_data);

	program_info.AddKernel(kernel_name);

	m_program = m_controller->AddProgram(program_info);
	m_program->Build();
	Logger::LogDebug(LOG_POS("Init"), "Build program");

	vertex_buffer = m_controller->NewReadWriteBuffer(elements, sizeof(float) * 4);
	normal_buffer = m_controller->NewReadWriteBuffer(elements, sizeof(float) * 4);
	vbo_buffer = m_controller->NewReadWriteBuffer(elements, STRIDE * sizeof(float), true);

	IComputeProgram::BindIndex ind{};
	ind.GlobalIndex = 0;
	m_program->KernelSetBuffer(kernel_name, vbo_buffer, ind);

	ind = {};
	ind.GlobalIndex = 1;
	m_program->KernelSetBuffer(kernel_name, vertex_buffer, ind);

	ind = {};
	ind.GlobalIndex = 2;
	m_program->KernelSetBuffer(kernel_name, normal_buffer, ind);

	m_program->FinishBuild();
	Logger::LogDebug(LOG_POS("Init"), "Finish Build program");
}

void Stitch_VBO::Stitch(int elements)
{
	m_program->RunKernel(elements, m_elements, 1, 1);

	Logger::LogDebug(LOG_POS("Stitch"), "Executed Kernel");
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
