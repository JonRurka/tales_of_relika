#pragma once

#include "stdafx.h"
#include "ForwardDeclarations.h"
#include "IComputeBuffer_private.h"

namespace DynamicCompute {
	namespace Compute {

		class IComputeProgram_private {
		public:

			enum class ProgramBuildState {
				None,
				Inited,
				Constructed,
				Built,
				BuildError
			};

			enum class FileType {
				Text_File,
				Binary_File,
				Text_Data,
				Binary_Data,
			};

			struct ProgramInfo {
			public:
				
				ProgramInfo()
					: m_name(""), m_type(FileType::Binary_File) {}

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

			virtual void AddIncludeDirectory(std::string directory) = 0;

			virtual void AddDefine(std::string name, std::string value) = 0;

			virtual int Build() = 0;

			virtual int FinishBuild() = 0;

			virtual int GetKernelID(std::string name) = 0;

			// Removed from Interface because it is not compatible with all frameworks, but is usefull in OpenCL/CUDA
			//virtual int KernelAddBuffer(std::string k_name, IComputeBuffer_private* buffer) = 0;

			virtual void KernelSetWorkGroupSize(std::string k_name, glm::uvec3 size) = 0;

			virtual int KernelSetBuffer(std::string k_name, IComputeBuffer_private* buffer, BindIndex arg) = 0;

			virtual int RunKernel(std::string k_name, int size_x, int size_y, int size_z) = 0;

			virtual int RunKernel(int kernel_id, int size_x, int size_y, int size_z) = 0;

			virtual int RunKernel(std::string k_name, int num, int size_x, int size_y, int size_z) = 0;

			virtual int RunKernel(int kernel_id, int num, int size_x, int size_y, int size_z) = 0;

			virtual void* GetKernelFunction(int kernel_id) = 0;

			virtual ProgramBuildState GetState() = 0;

			virtual int GetBuildResultCode() = 0;

			virtual std::string GetBuildErrorMessage() = 0;

			virtual std::string GetProgramName() = 0;

			virtual void Dispose() = 0;
		};
	}
}