#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "dynamic_compute.h"
#include "shared_structures.h"

using namespace DynamicCompute::Compute;

class GPUSort {
public:

	GPUSort(size_t size);

	//void Sort(std::vector<glm::vec4>& values);
	void Sort(glm::vec4* values, int num_elements, bool on_gpu = true);
	void Sort(std::vector<glm::vec4>& values, bool on_gpu = true);

private:

	struct Settings {
		unsigned int numEntries;
		unsigned int groupWidth;
		unsigned int groupHeight;
		unsigned int stepIndex;
	};

	std::vector<Settings> m_run_settings_entries;

	unsigned int m_size{ 0 };
	Vulkan_Device_Info m_device{};
	ComputeInterface::ControllerInfo m_controllerInfo{};
	IComputeController* m_controller{ nullptr };
	IComputeProgram* m_program{nullptr};

	IComputeBuffer* m_in_exec_settings_buffer;
	IComputeBuffer* m_in_settings_buffer;
	IComputeBuffer* m_entries_buffer;

	inline static const std::string LOG_LOC{ "GPU_SORT" };

};
