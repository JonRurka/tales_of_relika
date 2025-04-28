#include "ComputeProgram_OCL.h"

#include "ComputeBuffer_OCL.h"

using namespace DynamicCompute::Compute;
using namespace DynamicCompute::Compute::OCL;

#define ERROR_SIZE 10000
#define DEFAULT_BINARY_FILE_TYPE "spv"
#define DEFAULT_TEXT_FILE_TYPE "cl"

void ComputeProgram_OCL::Init(std::string name)
{
	m_program_name = name;

	//m_builder = new ProgramBuilder(m_context, m_program_name);
	m_program = m_context->Add_Program(m_program_name);

	m_cur_state = ProgramBuildState::Inited;
}

void ComputeProgram_OCL::AddIncludeDirectory(std::string directory)
{
	m_program->AddIncludeDirector(directory);
}

void ComputeProgram_OCL::AddDefine(std::string name, std::string value)
{
	m_program->AddConstant(name, value);
}

int ComputeProgram_OCL::Build()
{
	printf("ComputeController_OCL: Reading program from directory: %s\n", m_program_directory.c_str());

	switch (m_ftype) {
		case IComputeProgram_private::FileType::Binary_File:
		{
			std::string full_file_path = m_program_directory + m_program_name + "." + DEFAULT_BINARY_FILE_TYPE;
			printf("ComputeController_OCL: Adding program binary file: %s\n", full_file_path.c_str());
			BuildProgramFromBinary(full_file_path, m_kernel_names);
			break;
		}
		case IComputeProgram_private::FileType::Text_File:
		{
			std::string full_file_path = m_program_directory + m_program_name + "." + DEFAULT_TEXT_FILE_TYPE;
			printf("ComputeController_OCL: Adding program source file: %s\n", full_file_path.c_str());
			BuildProgramFromSourceFile(full_file_path, m_kernel_names);

			break;
		}
		case IComputeProgram_private::FileType::Text_Data:
			printf("ComputeController_OCL: Adding program source.\n");
			BuildProgramFromSource(std::string(m_data.begin(), m_data.end()), m_kernel_names);
			break;
		case IComputeProgram_private::FileType::Binary_Data:
			printf("ComputeController_OCL: Adding program binary data\n");
			BuildProgramFromBinary(m_data.data(), m_data.size(), m_kernel_names);
			break;
	}
	return 0;
}

int ComputeProgram_OCL::GetKernelID(std::string name)
{
	if (m_kernel_name_to_id.count(name) <= 0) {
		printf("Kernel not found: %s\n", name.c_str());
		return -1;
	}

	return m_kernel_name_to_id[name];
}

void ComputeProgram_OCL::KernelSetWorkGroupSize(std::string k_name, glm::uvec3 size)
{
	
}

int ComputeProgram_OCL::KernelAddBuffer(std::string k_name, IComputeBuffer_private* buffer)
{
	if (m_kernel_name_to_id.count(k_name) <= 0) {
		printf("Kernel not found: %s\n", k_name.c_str());
		return -1;
	}

	int K_ID = GetKernelID(k_name);

	BindIndex ind{}; 
	ind.ParameterIndex = m_kernel_entries[K_ID].args;
	int res = KernelSetBuffer(k_name, buffer, ind);
	m_kernel_entries[K_ID].args++;

	return res;
}

int ComputeProgram_OCL::KernelSetBuffer(std::string k_name, IComputeBuffer_private* buffer, BindIndex arg)
{
	if (m_kernel_name_to_id.count(k_name) <= 0) {
		printf("Kernel not found: %s\n", k_name.c_str());
		return -1;
	}

	int K_ID = GetKernelID(k_name);

	// TODO: handle setting kernel entry arg if larger.

	ComputeBuffer* bfr = ((ComputeBuffer_OCL*)buffer)->GetBuffer();
	return BindKernel(bfr, m_kernel_entries[K_ID].kernel, arg.ParameterIndex);
}

int ComputeProgram_OCL::RunKernel(std::string k_name, int size_x, int size_y, int size_z)
{
	if (m_kernel_name_to_id.count(k_name) <= 0) {
		printf("Kernel not found: %s\n", k_name.c_str());
		return -1;
	}

	int K_ID = GetKernelID(k_name);

	return RunKernel(K_ID, size_x, size_y, size_z);
}

int ComputeProgram_OCL::RunKernel(int kernel_id, int size_x, int size_y, int size_z)
{
	if (kernel_id >= m_num_kernels)
	{
		printf("Kernel ID not found: %i\n", kernel_id);
		return -1;
	}

	size_t global[] = { std::max(size_x, 1) , std::max(size_y, 1) , std::max(size_z, 1) };
	return m_kernel_entries[kernel_id].kernel->Execute(3, global);
}

int ComputeProgram_OCL::RunKernel(std::string k_name, int num, int size_x, int size_y, int size_z)
{
	for (int i = 0; i < num; i++) {
		RunKernel(k_name, size_x, size_y, size_z);
	}
	return 0;
}

int ComputeProgram_OCL::RunKernel(int kernel_id, int num, int size_x, int size_y, int size_z)
{
	for (int i = 0; i < num; i++) {
		RunKernel(kernel_id, size_x, size_y, size_z);
	}
	return 0;
}

void* ComputeProgram_OCL::GetKernelFunction(int kernel_id)
{
	return nullptr;
}

void ComputeProgram_OCL::Dispose()
{
}



// Non-interface methods:

ComputeProgram_OCL::ComputeProgram_OCL(ComputeContext* context)
{
	m_context = context;
}

/*IComputeProgram_private::ProgramBuildState ComputeProgram_OCL::ConstructSourceProgram()
{
	if (m_cur_state == ProgramBuildState::Inited)
	{
		default_shaders.Load();

		for (auto s_name : default_shaders.m_source)
		{
			std::string source = default_shaders.GetCode(s_name);
			m_builder->AppendSource(source + "\n");
		}
		// TODO: Event for additional source code

		std::vector<std::string> f_defs = default_shaders.GetFunctionsDefinitions();
		// TODO: Event for additional function definitions
		for (auto def : f_defs)
		{
			m_builder->AppendSource(def);
		}
		m_builder->AppendSource("\n");

		for (auto f_name : default_shaders.m_functions)
		{
			m_builder->AddFunction(f_name, default_shaders.GetCode(f_name) + "\n");
		}
		// TODO: Event for additional functions

		for (auto k_name : default_shaders.m_kernels)
		{
			m_builder->AddKernel(k_name, default_shaders.GetCode(k_name) + "\n");
		}
		// TODO: Event for additional kernels

		default_shaders.Clear();
		m_cur_state = ProgramBuildState::Constructed;
	}

	return m_cur_state;
}*/

IComputeProgram_private::ProgramBuildState ComputeProgram_OCL::BuildProgramFromBinary(std::string file_path, std::vector<std::string> kernels)
{
	//m_builder->AddKernels(kernels);
	for (int i = 0; i < kernels.size(); i++)
	{
		m_kernels.push_back(kernels[i]);
	}

	//m_cl_build_res = m_builder->BuildFromBinary(file_path);
	char errorstr[ERROR_SIZE];
	int set_b_res = m_program->Set_Binary_File(file_path);
	if (set_b_res != 0)
	{
		m_cur_state = ProgramBuildState::BuildError;
		printf("BuildFromBinary(file) Failed at m_program->Set_Binary: %i\n", set_b_res);
		return m_cur_state;
	}

	int status = m_program->Build(errorstr, ERROR_SIZE);
	m_build_error = errorstr;
	if (status != 0)
	{
		m_cur_state = ProgramBuildState::BuildError;
		printf("BuildFromBinary(file) Failed at m_program->Build: %i\n", status);
		return m_cur_state;
	}

	for (int i = 0; i < m_kernels.size(); i++)
	{
		m_program->GetKernel(m_kernels[i]);
	}

	if (m_cl_build_res == 0)
	{
		printf("Program Built successfully!\n");

		// get kernels
		int kernel_res = InitKernelEntries();

		if (kernel_res == 0)
			m_cur_state = ProgramBuildState::Built;
		else
		{
			m_cur_state = ProgramBuildState::BuildError;
			m_build_error = "Kernel Error " + std::to_string(kernel_res);
		}
	}
	else
	{
		m_cur_state = ProgramBuildState::BuildError;
		m_build_error = m_build_error;
		printf("Got non-zero result from BuildFromBinary: %i\n", m_cl_build_res);
		//printf("Build Error: %s\n", m_builder->GetError().c_str());
	}

	//m_builder->Clear();

	return m_cur_state;
}

IComputeProgram_private::ProgramBuildState ComputeProgram_OCL::BuildProgramFromBinary(const void* binary, size_t length, std::vector<std::string> kernels)
{
	//m_builder->AddKernels(kernels);
	for (int i = 0; i < kernels.size(); i++)
	{
		m_kernels.push_back(kernels[i]);
	}

	//m_cl_build_res = m_builder->BuildFromBinary(binary, length);
	char errorstr[ERROR_SIZE];
	int set_b_res = m_program->Set_Binary(binary, length);
	if (set_b_res != 0)
	{
		m_cur_state = ProgramBuildState::BuildError;
		printf("BuildFromBinary(bin) Failed at m_program->Set_Binary: %i\n", set_b_res);
		return m_cur_state;
	}

	int status = m_program->Build(errorstr, ERROR_SIZE);
	m_build_error = errorstr;
	if (status != 0)
	{
		m_cur_state = ProgramBuildState::BuildError;
		printf("BuildFromBinary(bin) Failed at m_program->Build: %i\n", status);
		return m_cur_state;
	}
	for (int i = 0; i < m_kernels.size(); i++)
	{
		m_program->GetKernel(m_kernels[i]);
	}

	if (m_cl_build_res == 0)
	{
		printf("Program Built successfully!\n");

		// get kernels
		int kernel_res = InitKernelEntries();

		if (kernel_res == 0)
			m_cur_state = ProgramBuildState::Built;
		else
		{
			m_cur_state = ProgramBuildState::BuildError;
			m_build_error = "Kernel Error " + std::to_string(kernel_res);
		}
	}
	else
	{
		m_cur_state = ProgramBuildState::BuildError;
		m_build_error = m_build_error;
		printf("Got non-zero result from BuildFromBinary: %i\n", m_cl_build_res);
		//printf("Build Error: %s\n", m_builder->GetError().c_str());
	}

	//m_builder->Clear();
	return m_cur_state;
}

IComputeProgram_private::ProgramBuildState ComputeProgram_OCL::BuildProgramFromSource(std::string content, std::vector<std::string> kernels)
{
	//m_builder->AddKernels(kernels);
	for (int i = 0; i < kernels.size(); i++)
	{
		m_kernels.push_back(kernels[i]);
	}
	//m_builder->AppendSource(content);

	//m_cl_build_res = m_builder->BuildFromSource();
	char errorstr[ERROR_SIZE];
	m_program->Set_Source(content.c_str());
	m_cl_build_res = m_program->Build(errorstr, ERROR_SIZE);
	std::string error_msg = m_cl_build_res != 0 ? errorstr : "Program Built Successfully";

	if (m_cl_build_res == 0)
	{
		printf("Program Built successfully!\n");

		// get kernels
		int kernel_res = InitKernelEntries();

		if (kernel_res == 0)
			m_cur_state = ProgramBuildState::Built;
		else
		{
			m_cur_state = ProgramBuildState::BuildError;
			m_build_error = "Kernel Error " + std::to_string(kernel_res);
		}
	}
	else
	{
		m_cur_state = ProgramBuildState::BuildError;
		m_build_error = error_msg;
		printf("Got non-zero result from BuildFromBinary: %i\n", m_cl_build_res);
		//printf("Build Error: %s\n", m_builder->GetError().c_str());
	}

	//m_builder->Clear();

	return m_cur_state;
}

IComputeProgram_private::ProgramBuildState ComputeProgram_OCL::BuildProgramFromSourceFile(std::string file_path, std::vector<std::string> kernels)
{
	//m_builder->AddKernels(kernels);
	for (int i = 0; i < kernels.size(); i++)
	{
		m_kernels.push_back(kernels[i]);
	}

	//open file
	std::ifstream infile(file_path);
	std::vector<char> buffer;

	//get length of file
	infile.seekg(0, infile.end);
	size_t length = infile.tellg();
	infile.seekg(0, infile.beg);

	//read file
	if (length > 0 && length < 18446744073709551615) {
		buffer.resize(length);
		//buffer.insert(buffer.begin(), std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>());
		infile.read(&buffer[0], length);
	}
	else {
		printf("File not found: %s\n", file_path.c_str());
		return ProgramBuildState::BuildError;
	}

	std::string file_res(buffer.begin(), buffer.end());

	//m_builder->AppendSource(file_res);
	//m_cl_build_res = m_builder->BuildFromSource();
	char errorstr[ERROR_SIZE];
	m_program->Set_Source(file_res.c_str());
	m_cl_build_res = m_program->Build(errorstr, ERROR_SIZE);
	std::string error_msg = m_cl_build_res != 0 ? errorstr : "Program Built Successfully";

	if (m_cl_build_res == 0)
	{
		printf("Program Built successfully!\n");

		// get kernels
		int kernel_res = InitKernelEntries();

		if (kernel_res == 0)
			m_cur_state = ProgramBuildState::Built;
		else
		{
			m_cur_state = ProgramBuildState::BuildError;
			m_build_error = "Kernel Error " + std::to_string(kernel_res);
		}
	}
	else
	{
		m_cur_state = ProgramBuildState::BuildError;
		m_build_error = error_msg;
		printf("Got non-zero result from BuildFromBinary: %i\n", m_cl_build_res);
		//printf("Build Error: %s\n", m_builder->GetError().c_str());
	}

	//m_builder->Clear();

	return m_cur_state;
}

/*
IComputeProgram_private::ProgramBuildState ComputeProgram_OCL::BuildProgramFromInternalDepo()
{
	if (m_cur_state == ProgramBuildState::Constructed)
	{
		m_cl_build_res = m_builder->BuildFromSource();

		if (m_cl_build_res == 0)
		{
			printf("Program Built successfully!\n");

			// get kernels
			int kernel_res = InitKernelEntries();

			if (kernel_res == 0)
				m_cur_state = ProgramBuildState::Built;
			else
			{
				m_cur_state = ProgramBuildState::BuildError;
				m_build_error = "Kernel Error " + std::to_string(kernel_res);
			}
		}
		else
		{
			m_cur_state = ProgramBuildState::BuildError;
			m_build_error = m_builder->GetError();
			printf("Got non-zero result from BuildFromBinary: %i\n", m_cl_build_res);
			//printf("Build Error: %s\n", m_builder->GetError().c_str());
		}

		m_builder->Clear();
	}

	return m_cur_state;
}
*/


// private functions:

int ComputeProgram_OCL::BindKernel(ComputeBuffer* buffer, ComputeKernel* kernel, int arg)
{
	printf("Bind buffer to kernel: %i\n", arg);
	return kernel->SetBuffer(buffer, arg);
}

int ComputeProgram_OCL::InitKernelEntries()
{
	//std::vector<std::string> kernls = m_builder->GetKernels();

	m_num_kernels = m_kernels.size();

	if (m_kernel_entries != nullptr)
		delete[] m_kernel_entries;

	m_kernel_entries = new kernelEnt[m_num_kernels];

	for (int i = 0; i < m_num_kernels; i++)
	{
		std::string k_name = m_kernels[i];

		kernelEnt k_ent;
		k_ent.name = k_name;
		k_ent.kernel = m_program->GetKernel(k_name);//m_builder->GetKernel(k_name);
		k_ent.args = 0;

		int k_status = k_ent.kernel->GetStatus();

		if (k_status != 0)
		{
			printf("Kernel '%s' failed with Code %i.\n", k_ent.name.c_str(), k_status);
			return k_status;
		}

		m_kernel_name_to_id[k_ent.name] = i;
		m_kernel_entries[i] = k_ent;
	}

	// established kernels.
	// someKnownKernel = m_kernel_entries["coolKernel"];

	return 0;
}
