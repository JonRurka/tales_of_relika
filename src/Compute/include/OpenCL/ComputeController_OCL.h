#pragma once

#include "OCL_forwardDeclarations.h"
#include "Compute_Interface/PlatformStructures_private.h"
#include "ComputeProgram_OCL.h"
#include "ComputeEngine.h"


namespace DynamicCompute {
	namespace Compute {
		namespace OCL {
			
			class ComputeController_OCL : public IComputeController_private {
				friend class ComputeInterface_private;

			public:
				void Init(Platform platform, void* device, std::string program_dir);

				IComputeProgram_private* AddProgram(IComputeProgram_private::ProgramInfo info);

				IComputeProgram_private* GetProgram(std::string name);

				IComputeBuffer_private* NewReadBuffer(size_t numElements, size_t stride);

				IComputeBuffer_private* NewWriteBuffer(size_t numElements, size_t stride);

				IComputeBuffer_private* NewReadWriteBuffer(size_t numElements, size_t stride);

				void Dispose() {}

				// Non-interface methods:

				ComputeBuffer* NewBuffer(ComputeBuffer::Buffer_Type type, size_t length);

			private:
				ComputeController_OCL() {}

				static IComputeController_private* New();

				ComputeContext* m_context{ nullptr };
				std::string m_directory;

				std::map<std::string, ComputeProgram_OCL*> m_programs;

				static std::vector<ComputeController_OCL*> m_controllers;
			};

		}
	}
}