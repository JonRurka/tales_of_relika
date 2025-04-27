#include "GPUSort.h"

#include <algorithm>

#include "dynamic_compute.h"
#include "shared_structures.h"
#include "IVoxelBuilder_private.h"

#include "Utilities.h"
#include "Logger.h"

#define PROGRAM_NAME "sort"
#define KERNEL_NAME "main"

 

using namespace DynamicCompute::Compute;

namespace {
	int NextPowerOfTwo(int n) {
		n--;
		n |= n >> 1;   // Divide by 2^k for consecutive doublings of k up to 32,
		n |= n >> 2;   // and then or the results.
		n |= n >> 4;
		n |= n >> 8;
		n |= n >> 16;
		n++;           // The result is a number of 1 bits equal to the number
					   // of bits in the original number, plus 1. That's the
					   // next highest power of 2.
		return n;
	}
}

GPUSort::GPUSort(size_t size)
{
	m_size = size;

	std::vector<Vulkan_Device_Info> devices = ComputeInterface::GetSupportedDevices_Vulkan();
	m_device = devices[1];

	std::string shaderDir = "C:\\Users\\jrurka\\Source\\repos\\game_project\\resources\\shaders\\compute\\engine\\bitonic_merge_sort";

	m_controllerInfo.device = &m_device;
	m_controllerInfo.SetProgramDir(shaderDir);

	m_controller = ComputeInterface::GetComputeController(ComputeInterface::VULKAN, m_controllerInfo);

	IComputeProgram::ProgramInfo program_info = IComputeProgram::ProgramInfo(PROGRAM_NAME, IComputeProgram::FileType::Binary_File);
	program_info.AddKernel(KERNEL_NAME);

	m_program = m_controller->AddProgram(program_info);
	m_program->Build();
	m_program->KernelSetWorkGroupSize(KERNEL_NAME, glm::uvec3(128, 1, 1));

	m_in_exec_settings_buffer = m_controller->NewReadWriteBuffer(1, sizeof(float) * 4);
	m_in_settings_buffer = m_controller->NewReadBuffer(1024, sizeof(int) * 4);
	m_entries_buffer = m_controller->NewReadWriteBuffer(size, sizeof(float) * 4);

	IComputeProgram::BindIndex ind{};
	ind.GlobalIndex = 0;
	m_program->KernelSetBuffer(KERNEL_NAME, m_in_exec_settings_buffer, ind);

	ind = {};
	ind.GlobalIndex = 1;
	m_program->KernelSetBuffer(KERNEL_NAME, m_in_settings_buffer, ind);

	ind = {};
	ind.GlobalIndex = 2;
	m_program->KernelSetBuffer(KERNEL_NAME, m_entries_buffer, ind);

	m_program->FinishBuild();
}

//void GPUSort::Sort(std::vector<glm::vec4>& values)
void GPUSort::Sort(glm::vec4* values, int num_elements, bool on_gpu)
{
	float start = Utilities::Get_Time();

	if (!on_gpu) {

		std::vector<glm::vec4> val_test(values, values + m_size);

		// For descending order, change to:
		std::sort(val_test.begin(), val_test.end(),
			[](glm::vec4 a, glm::vec4 b) { return a.y > b.y; });

		memcpy(values, val_test.data(), m_size * sizeof(float) * 4);
	}
	else
	{
		float start = Utilities::Get_Time();

		//Settings run_settings{};
		//run_settings.numEntries = m_size;

		int numStages = (int)log2(NextPowerOfTwo(num_elements));

		m_run_settings_entries.clear();

		for (int stageIndex = 0; stageIndex < numStages; stageIndex++) {
			for (int stepIndex = 0; stepIndex < stageIndex + 1; stepIndex++)
			{
				Settings run_settings{};
				run_settings.numEntries = num_elements;

				// Calculate some pattern stuff
				int groupWidth = 1 << (stageIndex - stepIndex);
				int groupHeight = 2 * groupWidth - 1;
				run_settings.groupWidth = (unsigned int)groupWidth;
				run_settings.groupHeight = (unsigned int)groupHeight;
				run_settings.stepIndex = (unsigned int)stepIndex;
				m_run_settings_entries.push_back(run_settings);
			}
		}

		m_in_settings_buffer->SetData(m_run_settings_entries.data(), m_run_settings_entries.size());
		Logger::LogDebug(LOG_POS("Sort"), "Number of executions: %i, Number of elements: %i", m_run_settings_entries.size(), num_elements);
	
		glm::uvec4 exec_val(0.0f, m_run_settings_entries.size(), 0.0f, 0.0f);

		m_in_exec_settings_buffer->SetData(&exec_val);
		m_entries_buffer->SetData(values, num_elements);


		m_program->RunKernel(KERNEL_NAME, NextPowerOfTwo(num_elements), 1, 1);

		// if values size is greater than the size, rebuild.

		// Launch each step of the sorting algorithm (once the previous step is complete)
		// Number of steps = [log2(n) * (log2(n) + 1)] / 2
		// where n = nearest power of 2 that is greater or equal to the number of inputs

		/*int numStages = (int)log2(NextPowerOfTwo(m_size));

		for (int stageIndex = 0; stageIndex < numStages; stageIndex++) {
			for (int stepIndex = 0; stepIndex < stageIndex + 1; stepIndex++) 
			{
				// Calculate some pattern stuff
				int groupWidth = 1 << (stageIndex - stepIndex);
				int groupHeight = 2 * groupWidth - 1;
				run_settings.groupWidth = (unsigned int)groupWidth;
				run_settings.groupHeight = (unsigned int)groupHeight;
				run_settings.stepIndex = (unsigned int)stepIndex;
				m_in_settings_buffer->SetData(&run_settings);
				m_program->RunKernel(KERNEL_NAME, NextPowerOfTwo(m_size), 1, 1);
			}
		}*/

		//glm::vec4* tmp = new glm::vec4[m_size];
		//memset(tmp, 0, run_settings.numEntries * sizeof(float) * 4);


		m_in_exec_settings_buffer->GetData(&exec_val);
		m_entries_buffer->GetData(values, num_elements);
		Logger::LogDebug(LOG_POS("Sort"), "Executed %u times.", exec_val.x);


		//values.clear();
		//values = std::vector<glm::vec4>(tmp, tmp + run_settings.numEntries);
		//delete[] tmp;
	}

	float stop = Utilities::Get_Time();
	Logger::LogDebug(LOG_POS("Sort"), "Sort time: %f ms", (stop - start) * 1000);
	
}

void GPUSort::Sort(std::vector<glm::vec4>& values, bool on_gpu)
{
	float start = Utilities::Get_Time();

	if (!on_gpu) {
		std::sort(values.begin(), values.end(),
			[](glm::vec4 a, glm::vec4 b) { return a.y > b.y; });
	}

	float stop = Utilities::Get_Time();
	Logger::LogDebug(LOG_POS("Sort"), "Sort time: %f ms", (stop - start) * 1000);
}
