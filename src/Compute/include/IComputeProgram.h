#pragma once

#include "std_includes.h"

namespace DynamicCompute {
	namespace Compute {

		class IComputeProgram {
			friend class IComputeController;
		public:

			enum class ProgramBuildState {
				None,
				Inited,
				Constructed,
				Built,
				BuildError
			};

			enum class FileType {
				Text,
				Binary,
				Raw
			};

			struct ProgramInfo {
			public:

				ProgramInfo() 
					: m_name(""), m_type(FileType::Binary) {}

				ProgramInfo(std::string name, FileType type)
					: m_name(name), m_type(type) {}

				void AddKernel(std::string name) { m_kernels.push_back(name); }

				void SetData(std::vector<uint8_t> data) { m_data = data; }

				std::string Name() { return m_name; }

				FileType Type() {
					return m_type;
				}

				std::vector<uint8_t> Data() {
					return m_data;
				}

				std::vector<std::string> Kernels() { return m_kernels; }

			private:
				std::string m_name;
				FileType m_type;
				std::vector<std::string> m_kernels;
				std::vector<uint8_t> m_data;
			};

			struct BindIndex {
				// Used in kernels where buffers are bound to function parameters
				int ParameterIndex;

				// Used in kernels where buffers are bound in the global scope
				int GlobalIndex;

				// Used in DirectX kernels, where read/write buffers have seperate index counters.
				int RegisterIndex;
			};

			// Removed from interface because this is implementation dependent.
			//virtual void Init(std::string name) = 0;

			void AddIncludeDirectory(std::string directory);

			void AddDefine(std::string name, std::string value);

			int Build();

			int FinishBuild();

			int GetKernelID(std::string name);

			// Removed from Interface because it is not compatible with all frameworks, but is usefull in OpenCL/CUDA
			//int KernelAddBuffer(std::string k_name, IComputeBuffer_private* buffer) = 0;

			void KernelSetWorkGroupSize(std::string k_name, int size);

			int KernelSetBuffer(std::string k_name, IComputeBuffer* buffer, BindIndex arg);

			int RunKernel(std::string k_name, int size_x, int size_y, int size_z);

			int RunKernel(int kernel_id, int size_x, int size_y, int size_z);

			void* GetKernelFunction(int kernel_id);

			ProgramBuildState GetState();

			int GetBuildResultCode();

			std::string GetBuildErrorMessage();

			std::string GetProgramName();

			void Dispose();


		private:

			IComputeProgram(IComputeProgram_private* p) { p_inst = p; }

			IComputeProgram_private* p_inst;
		};

	}
}