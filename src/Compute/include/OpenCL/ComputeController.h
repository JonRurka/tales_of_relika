#pragma once

#include "ComputeEngine.h"
#include "ProgramBuilder.h"
//#include "ShaderDepository.h"

namespace Dynamics_IO_Testbench {
	namespace Compute {
		namespace OCL {

			class ComputeController {
			public:
				enum class ComputeState {
					None,
					Inited,
					Constructed,
					Built,
					BuildError
				};

				ComputeController(cl_platform_id pltform, cl_device_id device, std::string inc_dir);

				void TestCuda();

				ProgramBuilder* GetProgramBuilder() { return m_builder; }

				ComputeState ConstructSourceProgram();

				ComputeState BuildProgramFromBinary(std::string file_path, std::vector<std::string> kernels);

				ComputeState BuildProgramFromBinary(const void* binary, size_t length, std::vector<std::string> kernels);

				ComputeState BuildProgramFromSource();

				ComputeBuffer* NewReadBuffer(size_t length);

				ComputeBuffer* NewWriteBuffer(size_t length);

				ComputeBuffer* NewReadWriteBuffer(size_t length);

				ComputeBuffer* NewBuffer(ComputeBuffer::Buffer_Type type, size_t length);

				int KernelAddBuffer(std::string k_name, ComputeBuffer* buffer);

				int BindKernel(std::string k_name, ComputeBuffer* buffer, int arg);

				int RunKernel(std::string k_name, int size_x, int size_y, int size_z);

				ComputeState GetState() { return m_cur_state; }

				int Get_CL_Build_Code() { return m_cl_build_res; }

			private:

				struct kernelEnt {
					std::string name;
					ComputeKernel* kernel;
					int args;
				};

				int BindKernel(ComputeBuffer* buffer, ComputeKernel* kernel, int arg);

				void InitKernelEntries();

				ComputeContext* m_context{ nullptr };
				ProgramBuilder* m_builder{ nullptr };
				ComputeState m_cur_state{ ComputeState::None };
				std::string m_build_error;
				int m_cl_build_res = { 0 };
				ShaderDepository default_shaders;


				// known kernels

				// other kernels
				std::map<std::string, kernelEnt> m_kernel_entries;

			};


		}
	}
}
