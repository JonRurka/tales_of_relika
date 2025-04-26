#include "ComputeController_OCL.h"

#include "ComputeBuffer_OCL.h"

#include <boost/algorithm/string.hpp>

using namespace DynamicCompute::Compute;
using namespace DynamicCompute::Compute::OCL;

std::vector<ComputeController_OCL*> ComputeController_OCL::m_controllers;


void ComputeController_OCL::Init(Platform platform, void* device, std::string program_dir)
{
	m_directory = program_dir + "/"; // TODO: check end of program_dir string for '/'

	
	ComputeEngine::Init(platform, m_directory + "include");

	OpenCL_Device_Info info = *(OpenCL_Device_Info*)device;

	// TODO: Add ability to add OpenGL shared context
	m_context = ComputeEngine::GetNewContext(info);
}

IComputeProgram_private* ComputeController_OCL::AddProgram(IComputeProgram_private::ProgramInfo info)
{
	std::string name = info.Name();
	std::vector<std::string> kernels = info.Kernels();

	std::string program_dir = m_directory + name = "/";

	ComputeProgram_OCL* program = new ComputeProgram_OCL(m_context);
	program->Init(name);
	program->SetProgramDirectory(m_directory);
	program->SetFileType(info.Type());
	program->SetKernelNames(kernels);

	
	//printf("ComputeController_OCL: Reading program from directory: %s\n", m_directory.c_str());
	
	/*switch (info.Type()) {
		case IComputeProgram_private::FileType::Binary:
		{
			std::string full_file_path = m_directory + name + "." + DEFAULT_BINARY_FILE_TYPE;
			printf("ComputeController_OCL: Adding program binary file: %s\n", full_file_path.c_str());
			program->BuildProgramFromBinary(full_file_path, kernels);
			break;
		}
		case IComputeProgram_private::FileType::Text:
		{
			std::string full_file_path = m_directory + name + "." + DEFAULT_TEXT_FILE_TYPE;
			printf("ComputeController_OCL: Adding program source file: %s\n", full_file_path.c_str());
			program->BuildProgramFromSourceFile(full_file_path, kernels);
			break;
		}
	}*/

	m_programs[name] = program;

	return (IComputeProgram_private*)program;
}

IComputeProgram_private* ComputeController_OCL::GetProgram(std::string name)
{
	if (m_programs.count(name) <= 0) {
		printf("Program not found: %s\n", name.c_str());
		return nullptr;
	}

	return m_programs[name];
}

IComputeBuffer_private* ComputeController_OCL::NewReadBuffer(size_t numElements, size_t stride, bool external)
{
	return new ComputeBuffer_OCL(ComputeController_OCL::NewBuffer(ComputeBuffer::Buffer_Type::READ, numElements * stride, external));
}

IComputeBuffer_private* ComputeController_OCL::NewWriteBuffer(size_t numElements, size_t stride, bool external)
{
	return new ComputeBuffer_OCL(ComputeController_OCL::NewBuffer(ComputeBuffer::Buffer_Type::Write, numElements * stride, external));
}

IComputeBuffer_private* ComputeController_OCL::NewReadWriteBuffer(size_t numElements, size_t stride, bool external)
{
	return new ComputeBuffer_OCL(ComputeController_OCL::NewBuffer(ComputeBuffer::Buffer_Type::Read_Write, numElements * stride, external));
}


// Non-interface methods:

ComputeBuffer* ComputeController_OCL::NewBuffer(ComputeBuffer::Buffer_Type type, size_t length, bool external)
{
	return m_context->GetBuffer(type, length, external);
}

IComputeController_private* DynamicCompute::Compute::OCL::ComputeController_OCL::New()
{
	int index = m_controllers.size();
	m_controllers.resize(index + 1);
	m_controllers[index] = new ComputeController_OCL();
	return m_controllers[index];
}
