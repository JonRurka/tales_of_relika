#include "ComputeProgram_VK.h"

#include "ComputeEngine.h"
#include "ComputeBuffer_VK.h"

using namespace DynamicCompute::Compute;
using namespace DynamicCompute::Compute::VK;

#define DEFAULT_BINARY_FILE_TYPE "spv"

void ComputeProgram_VK::Init(std::string name)
{
	m_program_name = name;
	m_cur_state = ProgramBuildState::Inited;
	m_program = m_context->Add_Program(name);
}

int DynamicCompute::Compute::VK::ComputeProgram_VK::Build()
{
	// ProgramInfo FileType is ignored because only SPIRV binary files are supported.
	std::string full_file_path = m_program_directory + m_program_name + "." + DEFAULT_BINARY_FILE_TYPE;

	//printf("ComputeController_VK: Reading program from directory: %s\n", m_program_directory.c_str());

	switch (m_ftype) {
	case FileType::Binary:
		printf("ComputeController_VK: Adding program binary file: %s\n", full_file_path.c_str());
		BuildProgramFromBinary(full_file_path, m_kernel_names);
		break;
	case FileType::Raw:
		BuildProgramFromBinary(m_raw_binary.data(), m_raw_binary.size(), m_kernel_names);
		break;
	}

	return 0;
}

int ComputeProgram_VK::FinishBuild()
{
	return m_program->Buildkernels();
}

int ComputeProgram_VK::GetKernelID(std::string name)
{
	if (m_kernel_name_to_id.count(name) <= 0) {
		printf("Kernel not found: %s\n", name.c_str());
		return -1;
	}

	return m_kernel_name_to_id[name];
}

void DynamicCompute::Compute::VK::ComputeProgram_VK::KernelSetWorkGroupSize(std::string k_name, glm::uvec3 size)
{
	if (m_kernel_name_to_id.count(k_name) <= 0) {
		printf("Kernel not found: %s\n", k_name.c_str());
		return;
	}

	int K_ID = GetKernelID(k_name);

	m_kernel_entries[K_ID].kernel->SetWorkGroupSize(size);
}

int ComputeProgram_VK::KernelSetBuffer(std::string k_name, IComputeBuffer_private* buffer, BindIndex arg)
{
	if (m_kernel_name_to_id.count(k_name) <= 0) {
		printf("Kernel not found: %s\n", k_name.c_str());
		return -1;
	}

	int K_ID = GetKernelID(k_name);

	// TODO: handle setting kernel entry arg if larger.

	ComputeBuffer* bfr = ((ComputeBuffer_VK*)buffer)->GetBuffer();
	return BindKernel(bfr, m_kernel_entries[K_ID].kernel, arg.GlobalIndex);
}

int ComputeProgram_VK::RunKernel(std::string k_name, int size_x, int size_y, int size_z)
{
	if (m_kernel_name_to_id.count(k_name) <= 0) {
		printf("Kernel not found: %s\n", k_name.c_str());
		return -1;
	}

	int K_ID = GetKernelID(k_name);

	return RunKernel(K_ID, size_x, size_y, size_z);
}

int ComputeProgram_VK::RunKernel(int kernel_id, int size_x, int size_y, int size_z)
{
	if (kernel_id >= m_num_kernels)
	{
		printf("Kernel ID not found: %i\n", kernel_id);
		return -1;
	}

	return m_kernel_entries[kernel_id].kernel->Execute(size_x, size_y, size_z);
}

int DynamicCompute::Compute::VK::ComputeProgram_VK::RunKernel(std::string k_name, int num, int size_x, int size_y, int size_z)
{
	if (m_kernel_name_to_id.count(k_name) <= 0) {
		printf("Kernel not found: %s\n", k_name.c_str());
		return -1;
	}

	int K_ID = GetKernelID(k_name);

	return RunKernel(K_ID, num, size_x, size_y, size_z);
}

int DynamicCompute::Compute::VK::ComputeProgram_VK::RunKernel(int kernel_id, int num, int size_x, int size_y, int size_z)
{
	if (kernel_id >= m_num_kernels)
	{
		printf("Kernel ID not found: %i\n", kernel_id);
		return -1;
	}

	return m_kernel_entries[kernel_id].kernel->ExecuteBatch(num, size_x, size_y, size_z);
}

void* ComputeProgram_VK::GetKernelFunction(int kernel_id)
{
	return nullptr;
}

void ComputeProgram_VK::Dispose()
{
	if (mDestroyed)
		return;

	/*
	TODO: This releases all VK resources, however, other than
	the vector of ComputeKernels, it does not free the memory 
	of the ComputeProgram object itself as it is still present in
	the std::vector<ComputeProgram> in the owning ComputeContext.
	If a user were to constantly create programs and destroy them
	for some reason, it would create a memory leak with an ever
	growing vector of disposed ComputePrograms. 

	std::vector<ComputeProgram> in ComputeContext should be replaced
	with an std::map
	*/

	m_program->Dispose();
	m_kernels.clear();

	if (m_kernel_entries != nullptr)
		delete[] m_kernel_entries;

	mDestroyed = true;
}

ComputeProgram_VK::ProgramBuildState ComputeProgram_VK::BuildProgramFromBinary(std::string file_path, std::vector<std::string> kernels)
{
	// compile the VKShaderModule from binary input
	int vk_set_res = m_program->Set_Binary_File(file_path);

	if (vk_set_res != 0) {
		m_cur_state = ProgramBuildState::BuildError;
		//m_build_error = m_builder->GetError();
		printf("Got non-zero result from Set_Binary: %i\n", m_vk_build_res);
		//printf("Build Error: %s\n", m_builder->GetError().c_str());

		return m_cur_state;
	}

	for (int i = 0; i < kernels.size(); i++)
	{
		m_kernels.push_back(kernels[i]);

		// Create kernel instances inside program
		m_program->GetKernel(m_kernels[i]);
	}

	//printf("Program Built successfully!\n");

	// setup kernels
	int kernel_res = InitKernelEntries();

	if (kernel_res == 0)
		m_cur_state = ProgramBuildState::Constructed;
	else
	{
		m_cur_state = ProgramBuildState::BuildError;
		m_build_error = "Kernel Error " + std::to_string(kernel_res);
	}

	return m_cur_state;
}

ComputeProgram_VK::ProgramBuildState ComputeProgram_VK::BuildProgramFromBinary(const void* binary, size_t length, std::vector<std::string> kernels)
{
	// compile the VKShaderModule from binary input
	int vk_set_res = m_program->Set_Binary(binary, length);

	if (vk_set_res != 0) {
		m_cur_state = ProgramBuildState::BuildError;
		//m_build_error = m_builder->GetError();
		printf("Got non-zero result from Set_Binary: %i\n", m_vk_build_res);
		//printf("Build Error: %s\n", m_builder->GetError().c_str());

		return m_cur_state;
	}

	for (int i = 0; i < kernels.size(); i++)
	{
		m_kernels.push_back(kernels[i]);

		// Create kernel instances inside program
		m_program->GetKernel(m_kernels[i]);
	}

	//printf("Program Built successfully!\n");

	// setup kernels
	int kernel_res = InitKernelEntries();

	if (kernel_res == 0)
		m_cur_state = ProgramBuildState::Constructed;
	else
	{
		m_cur_state = ProgramBuildState::BuildError;
		m_build_error = "Kernel Error " + std::to_string(kernel_res);
	}

	return m_cur_state;
}

ComputeProgram_VK::~ComputeProgram_VK()
{
	Dispose();
}


ComputeProgram_VK::ComputeProgram_VK(ComputeContext* context) {
	m_context = context;
}

int ComputeProgram_VK::BindKernel(ComputeBuffer* buffer, ComputeKernel* kernel, int arg)
{
	//printf("Bind buffer to kernel: %i\n", arg);
	return kernel->SetBuffer(buffer, arg);
}

int ComputeProgram_VK::InitKernelEntries()
{
	m_num_kernels = m_kernels.size();

	if (m_kernel_entries != nullptr)
		delete[] m_kernel_entries;

	m_kernel_entries = new kernelEnt[m_num_kernels];

	for (int i = 0; i < m_num_kernels; i++) {
		std::string k_name = m_kernels[i];

		kernelEnt k_ent;
		k_ent.name = k_name;
		k_ent.kernel = m_program->GetKernel(k_name);
		//k_ent.args = 0;

		int k_status = 0;//k_ent.kernel->GetStatus();

		if (k_status != 0)
		{
			printf("Kernel '%s' failed with Code %i.\n", k_ent.name.c_str(), k_status);
			return k_status;
		}

		m_kernel_name_to_id[k_ent.name] = i;
		m_kernel_entries[i] = k_ent;

	}

	return 0;
}
