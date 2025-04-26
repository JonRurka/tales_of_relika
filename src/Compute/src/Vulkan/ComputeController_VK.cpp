#include "ComputeController_VK.h"

#include "ComputeEngine_VK.h"
#include "ComputeProgram_VK.h"
#include "ComputeBuffer_VK.h"

using namespace DynamicCompute::Compute;
using namespace DynamicCompute::Compute::VK;


std::vector<ComputeController_VK*> ComputeController_VK::m_controllers;

void ComputeController_VK::Init(Platform platform, void* device, std::string program_dir)
{
	m_directory = program_dir + "/"; // TODO: check end of program_dir string for '/'

	if (!ComputeEngine::IsInitialized()) {
		int res = ComputeEngine::Init(m_directory);

		if (res != 0) {
			printf("Failed to initialize Vulkan\n");
		}
	}

	Vulkan_Device_Info info = *(Vulkan_Device_Info*)device;

	m_context = ComputeEngine::GetNewContext(info);
}

IComputeProgram_private* ComputeController_VK::AddProgram(IComputeProgram_private::ProgramInfo info)
{
	std::string name = info.Name();
	std::vector<std::string> kernels = info.Kernels();

	m_programs[name] = new ComputeProgram_VK(m_context);

	m_programs[name]->Init(name);
	m_programs[name]->SetProgramDirectory(m_directory);
	m_programs[name]->SetFileType(info.Type());
	m_programs[name]->SetBinaryData(info.Data());
	m_programs[name]->SetKernelNames(kernels);


	// --- BUILD MOVED TO program->Build()
	// ProgramInfo FileType is ignored because only SPIRV binary files are supported.
	//std::string full_file_path = m_directory + name + "." + DEFAULT_BINARY_FILE_TYPE;

	//printf("ComputeController_VK: Reading program from directory: %s\n", m_directory.c_str());

	//printf("ComputeController_OCL: Adding program binary file: %s\n", full_file_path.c_str());
	//m_programs[name]->BuildProgramFromBinary(full_file_path, kernels);

	return m_programs[name];
}

/*IComputeProgram_private* DynamicCompute::Compute::VK::ComputeController_VK::AddProgram(std::string name, std::string kernel, std::vector<uint8_t> data)
{
	std::vector<std::string> kernels;
	kernels.push_back(kernel);

	m_programs[name] = new ComputeProgram_VK(m_context);

	m_programs[name]->Init(name);
	m_programs[name]->SetFileType(IComputeProgram_private::FileType::Raw);
	m_programs[name]->SetBinaryData(data);
	m_programs[name]->SetKernelNames(kernels);

	return m_programs[name];
}*/

IComputeProgram_private* ComputeController_VK::GetProgram(std::string name)
{
	if (m_programs.count(name) <= 0) {
		printf("Program not found: %s\n", name.c_str());
		return nullptr;
	}

	return m_programs[name];
}

IComputeBuffer_private* ComputeController_VK::NewReadBuffer(size_t numElements, size_t stride, bool external)
{
	ComputeBuffer* bf = ComputeController_VK::NewBuffer((uint32_t)ComputeBuffer::Buffer_Type::READ, numElements * stride, external);
	ComputeBuffer_VK* bf_vk = new ComputeBuffer_VK(bf);
	return bf_vk;
}

IComputeBuffer_private* ComputeController_VK::NewWriteBuffer(size_t numElements, size_t stride, bool external)
{
	ComputeBuffer* bf = ComputeController_VK::NewBuffer((uint32_t)ComputeBuffer::Buffer_Type::Write, numElements * stride, external);
	return new ComputeBuffer_VK(bf);
}

IComputeBuffer_private* ComputeController_VK::NewReadWriteBuffer(size_t numElements, size_t stride, bool external)
{
	return new ComputeBuffer_VK(ComputeController_VK::NewBuffer((uint32_t)ComputeBuffer::Buffer_Type::Read_Write, numElements * stride, external));
}

ComputeBuffer* ComputeController_VK::NewBuffer(uint32_t type, size_t length, bool external)
{
	return m_context->CreateBuffer((ComputeBuffer::Buffer_Type)type, length, external);
}

void ComputeController_VK::Dispose()
{
	if (mDestroyed)
		return;

	for (auto& [key, value] : m_programs) {
		delete value;
	}

	m_programs.clear();
	m_context->Dispose();

	mDestroyed = true;
}

ComputeController_VK::~ComputeController_VK()
{
	Dispose();
}

IComputeController_private* ComputeController_VK::New()
{
	int index = m_controllers.size();
	m_controllers.resize(index + 1);
	m_controllers[index] = new ComputeController_VK();
	return m_controllers[index];
}

void ComputeController_VK::DisposePlatform() {
	if (ComputeEngine::IsInitialized()) {
		ComputeEngine::Dispose();
	}
}

void ComputeController_VK::Close()
{
	m_controllers.clear();
	DisposePlatform();
}
