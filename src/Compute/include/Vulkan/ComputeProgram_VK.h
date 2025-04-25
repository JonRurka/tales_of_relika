#pragma once

#include "InterfaceIncludes_private.h"

//#include "ProgramBuilder.h"

namespace DynamicCompute {
	namespace Compute {
		namespace VK {

			class ComputeContext;
			class ComputeEngine;
			class ComputeKernel;
			class ComputeBuffer;
			class ComputeProgram;

			class ComputeBuffer_VK;
			class ComputeController_VK;

			class ComputeProgram_VK : public IComputeProgram_private {
				friend class ComputeController_VK;

			public:
				// Interface methods:

				void Init(std::string name);

				// Only SPIRV files supported at this time. If/When source file loading
				// is added, this will be implemented.
				void AddIncludeDirectory(std::string directory){} 

				void AddDefine(std::string name, std::string value) {}

				int Build();

				int FinishBuild();

				int GetKernelID(std::string name);

				void KernelSetWorkGroupSize(std::string k_name, glm::uvec3 size);

				int KernelSetBuffer(std::string k_name, IComputeBuffer_private* buffer, BindIndex arg);

				int RunKernel(std::string k_name, int size_x, int size_y, int size_z);

				int RunKernel(int kernel_id, int size_x, int size_y, int size_z);

				int RunKernel(std::string k_name, int num, int size_x, int size_y, int size_z);

				int RunKernel(int kernel_id, int num, int size_x, int size_y, int size_z);

				void* GetKernelFunction(int kernel_id);

				ProgramBuildState GetState() { return m_cur_state; }

				int GetBuildResultCode() { return m_vk_build_res; }

				std::string GetBuildErrorMessage() { return m_build_error; }

				std::string GetProgramName() { return m_program_name; }

				void Dispose();


				// Non-interface methods:

				void SetProgramDirectory(std::string dir) {
					m_program_directory = dir;
				}
				void SetFileType(FileType ftype) {
					m_ftype = ftype;
				}
				void SetKernelNames(std::vector<std::string> kernel_names) {
					m_kernel_names = std::vector<std::string>(kernel_names);
				}
				void SetBinaryData(std::vector<uint8_t> data) {
					m_raw_binary = data;
				}

				ProgramBuildState BuildProgramFromBinary(std::string file_path, std::vector<std::string> kernels);

				ProgramBuildState BuildProgramFromBinary(const void* binary, size_t length, std::vector<std::string> kernels);


				ComputeProgram_VK(){}
				~ComputeProgram_VK();

			private:

				struct kernelEnt {
					std::string name;
					ComputeKernel* kernel;
					//int args;
				};

				ComputeProgram_VK(ComputeContext* context);

				int BindKernel(ComputeBuffer* buffer, ComputeKernel* kernel, int arg);

				int InitKernelEntries();

				ComputeContext* m_context{ nullptr };

				std::string m_program_name{""};
				std::string m_program_directory;
				FileType m_ftype;
				std::vector<uint8_t> m_raw_binary;
				std::vector<std::string> m_kernel_names;

				ProgramBuildState m_cur_state{ProgramBuildState::None};
				int m_vk_build_res { 0 };
				std::string m_build_error{""};

				size_t m_num_kernels{ 0 };
				kernelEnt* m_kernel_entries{ nullptr };
				std::map<std::string, int> m_kernel_name_to_id;

				// program building
				std::vector<std::string> m_kernels;
				ComputeProgram* m_program{ nullptr };

				bool mDestroyed{ false };
			};
		}
	}
}