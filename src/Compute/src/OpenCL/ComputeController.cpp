#include "ComputeController.h"
//#include "CudaTestRun.h"


using namespace Dynamics_IO_Testbench::Compute::OCL;

ComputeController::ComputeController(cl_platform_id pltform, cl_device_id device, std::string inc_dir)
{
	ComputeEngine::Init(pltform, device, inc_dir);

	m_context = ComputeEngine::GetNewContext();

	m_builder = new ProgramBuilder(m_context, "program");

	default_shaders.Load();

	m_cur_state = ComputeState::Inited;
}

void ComputeController::TestCuda()
{
	//RunCudaTest();
}

ComputeController::ComputeState ComputeController::ConstructSourceProgram()
{
	if (m_cur_state == ComputeState::Inited)
	{
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
		m_cur_state = ComputeState::Constructed;
	}

	return m_cur_state;
}

ComputeController::ComputeState ComputeController::BuildProgramFromBinary(std::string file_path, std::vector<std::string> kernels)
{

	m_builder->AddKernels(kernels);

	m_cl_build_res = m_builder->BuildFromBinary(file_path);

	if (m_cl_build_res == 0)
	{
		printf("Program Built successfully!\n");

		// get kernels
		InitKernelEntries();

		m_builder->Clear();
		m_cur_state = ComputeState::Built;
	}
	else
	{
		m_cur_state = ComputeState::BuildError;
		m_build_error = m_builder->GetError();
		printf("Got non-zero result from BuildFromBinary: %i\n", m_cl_build_res);
		//printf("Build Error: %s\n", m_builder->GetError().c_str());
	}

	return m_cur_state;
}

ComputeController::ComputeState ComputeController::BuildProgramFromBinary(const void* binary, size_t length, std::vector<std::string> kernels)
{
	m_builder->AddKernels(kernels);

	m_cl_build_res = m_builder->BuildFromBinary(binary, length);

	if (m_cl_build_res == 0)
	{
		printf("Program Built successfully!\n");

		// get kernels
		InitKernelEntries();

		m_builder->Clear();
		m_cur_state = ComputeState::Built;
	}
	else
	{
		m_cur_state = ComputeState::BuildError;
		m_build_error = m_builder->GetError();
		printf("Got non-zero result from BuildFromBinary: %i\n", m_cl_build_res);
		//printf("Build Error: %s\n", m_builder->GetError().c_str());
	}

	return m_cur_state;
}

ComputeController::ComputeState ComputeController::BuildProgramFromSource()
{
	if (m_cur_state == ComputeState::Constructed)
	{
		m_cl_build_res = m_builder->BuildFromSource();

		if (m_cl_build_res == 0)
		{
			printf("Program Built successfully!\n");
			
			// get kernels
			InitKernelEntries();


			m_builder->Clear();
			m_cur_state = ComputeState::Built;
		}
		else
		{
			m_cur_state = ComputeState::BuildError;
			m_build_error = m_builder->GetError();
			printf("Got non-zero result from BuildFromBinary: %i\n", m_cl_build_res);
			//printf("Build Error: %s\n", m_builder->GetError().c_str());
		}
	}

	return m_cur_state;
}

ComputeBuffer* ComputeController::NewReadBuffer(size_t length)
{
	return ComputeController::NewBuffer(ComputeBuffer::Buffer_Type::READ, length);
}

ComputeBuffer* ComputeController::NewWriteBuffer(size_t length)
{
	return ComputeController::NewBuffer(ComputeBuffer::Buffer_Type::Write, length);
}

ComputeBuffer* ComputeController::NewReadWriteBuffer(size_t length)
{
	return ComputeController::NewBuffer(ComputeBuffer::Buffer_Type::Read_Write, length);
}

ComputeBuffer* ComputeController::NewBuffer(ComputeBuffer::Buffer_Type type, size_t length)
{
	return m_context->GetBuffer(type, length);
}

int ComputeController::KernelAddBuffer(std::string k_name, ComputeBuffer* buffer)
{
	if (m_kernel_entries.count(k_name) <= 0) {
		printf("Kernel not found: %s\n", k_name.c_str());
		return -1;
	}

	int res = BindKernel(k_name, buffer, m_kernel_entries[k_name].args);
	m_kernel_entries[k_name].args++;
	return res;
}

int ComputeController::BindKernel(std::string k_name, ComputeBuffer* buffer, int arg)
{
	if (m_kernel_entries.count(k_name) <= 0) {
		printf("Kernel not found: %s\n", k_name.c_str());
		return -1;
	}

	return BindKernel(buffer, m_kernel_entries[k_name].kernel, arg);
}

int ComputeController::RunKernel(std::string k_name, int size_x, int size_y, int size_z)
{
	if (m_kernel_entries.count(k_name) <= 0)
		return -1;

	size_t global[] = { std::max(size_x, 1) , std::max(size_y, 1) , std::max(size_z, 1) };
	return m_kernel_entries[k_name].kernel->Execute(0, 3, global);
}

int ComputeController::BindKernel(ComputeBuffer* buffer, ComputeKernel* kernel, int arg)
{
	printf("Bind buffer to kernel: %i\n", arg);
	return kernel->SetBuffer(0, buffer, arg);
}

void ComputeController::InitKernelEntries()
{
	std::vector<std::string> kernls = m_builder->GetKernels();

	for (int i = 0; i < kernls.size(); i++)
	{
		std::string k_name = kernls[i];

		kernelEnt k_ent;
		k_ent.name = k_name;
		k_ent.kernel = m_builder->GetKernel(k_name);
		k_ent.args = 0;

		m_kernel_entries[k_ent.name] = k_ent;
	}

	// established kernels.
	// someKnownKernel = m_kernel_entries["coolKernel"];
}
