#pragma once

#include "ComputeEngine.h"

#define ERROR_SIZE 10000

namespace DynamicCompute {
	namespace Compute {
		namespace OCL {

			class ProgramBuilder {
			public:
				ProgramBuilder(ComputeContext* c_context, std::string name);

				void AddConstant(std::string name, std::string value);

				void AddFunction(std::string name, std::string f_source);

				void AddKernel(std::string name, std::string f_source);

				void AddKernel(std::string name);

				void AddKernels(std::vector<std::string> kernels);

				void AppendSource(std::string f_source);

				std::string GetError() { return m_error_msg; }

				int BuildFromSource();

				int BuildFromBinary(std::string file_path);

				int BuildFromBinary(const void* binary, size_t length);

				void Clear() { m_source.clear(); }

				ComputeProgram* GetProgram() { return m_program; }

				ComputeKernel* GetKernel(std::string name);

				std::vector<std::string> GetKernels() { return m_kernels; }

				std::string GetFullSource() { return m_source; }

			private:
				std::string m_name = "";
				std::string m_source = "";
				std::string m_error_msg = "";
				ComputeContext* m_context{ nullptr };
				ComputeProgram* m_program{ nullptr };
				std::vector<std::string> m_kernels;
			};

		}
	}
}
