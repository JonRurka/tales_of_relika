#pragma once

#include "OCL_forwardDeclarations.h"
#include "InterfaceIncludes_private.h"
#include "ComputeEngine_OCL.h"
//#include "ProgramBuilder.h"
//#include "ShaderDepository.h"

namespace DynamicCompute {
	namespace Compute {
		namespace OCL {

			class ComputeProgram_OCL : public IComputeProgram_private {
				friend class ComputeController_OCL;

			public:

				// Interface methods:

				void Init(std::string name);

				void AddIncludeDirectory(std::string directory) override;

				void AddDefine(std::string name, std::string value) override;

				int Build() override;

				int FinishBuild() override { return 0; }

				int GetKernelID(std::string name) override;

				void KernelSetWorkGroupSize(std::string k_name, glm::uvec3 size) override;

				int KernelAddBuffer(std::string k_name, IComputeBuffer_private* buffer);

				int KernelSetBuffer(std::string k_name, IComputeBuffer_private* buffer, BindIndex arg) override;

				int RunKernel(std::string k_name, int size_x, int size_y, int size_z) override;

				int RunKernel(int kernel_id, int size_x, int size_y, int size_z) override;

				int RunKernel(std::string k_name, int num, int size_x, int size_y, int size_z);

				int RunKernel(int kernel_id, int num, int size_x, int size_y, int size_z);

				void* GetKernelFunction(int kernel_id) override;

				ProgramBuildState GetState() override { return m_cur_state; }

				int GetBuildResultCode() override { return m_cl_build_res; }

				std::string GetBuildErrorMessage() override { return m_build_error; }

				std::string GetProgramName() override { return m_program_name; }

				void Dispose() override;


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
				void SetData(std::vector<uint8_t> data) {
					m_data = data;
				}

				//ProgramBuilder* GetProgramBuilder() { return m_builder; }

				//ProgramBuildState ConstructSourceProgram();

				ProgramBuildState BuildProgramFromBinary(std::string file_path, std::vector<std::string> kernels);

				ProgramBuildState BuildProgramFromBinary(const void* binary, size_t length, std::vector<std::string> kernels);

				ProgramBuildState BuildProgramFromSource(std::string content, std::vector<std::string> kernels);

				ProgramBuildState BuildProgramFromSourceFile(std::string file_path, std::vector<std::string> kernels);

				//ProgramBuildState BuildProgramFromInternalDepo();

			private:

				struct kernelEnt {
					std::string name;
					ComputeKernel* kernel;
					int args;
				};

				ComputeProgram_OCL(ComputeContext* context);

				int BindKernel(ComputeBuffer* buffer, ComputeKernel* kernel, int arg);

				int InitKernelEntries();

				std::vector<std::string> m_kernels;

				ComputeProgram* m_program{ nullptr };

				std::string m_program_name;
				std::string m_program_directory;
				FileType m_ftype;
				std::vector<std::string> m_kernel_names;

				std::vector<uint8_t> m_data;

				ComputeContext* m_context{ nullptr };
				//ProgramBuilder* m_builder{ nullptr };
				ProgramBuildState m_cur_state{ ProgramBuildState::None };

				std::string m_build_error = "";
				int m_cl_build_res = { 0 };

				//ShaderDepository default_shaders;

				size_t m_num_kernels;
				kernelEnt* m_kernel_entries{ nullptr };
				std::map<std::string, int> m_kernel_name_to_id;
			};
		}
	}
}