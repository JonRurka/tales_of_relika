#pragma once

#include "OCL_forwardDeclarations.h"
#include "PlatformStructures_private.h"
#include "ComputeProgram_OCL.h"
#include "ComputeEngine_OCL.h"


namespace DynamicCompute {
	namespace Compute {
		namespace OCL {
			
			class ComputeController_OCL : public IComputeController_private {
				friend class ComputeInterface_private;

			public:
				void Init(Platform platform, void* device, std::string program_dir) override;

				IComputeProgram_private* AddProgram(IComputeProgram_private::ProgramInfo info) override;

				IComputeProgram_private* GetProgram(std::string name) override;

				IComputeBuffer_private* NewReadBuffer(size_t numElements, size_t stride, bool external = false) override;

				IComputeBuffer_private* NewWriteBuffer(size_t numElements, size_t stride, bool external = false) override;

				IComputeBuffer_private* NewReadWriteBuffer(size_t numElements, size_t stride, bool external = false) override;

				void Dispose() {}

				// Non-interface methods:

				ComputeBuffer* NewBuffer(ComputeBuffer::Buffer_Type type, size_t length, bool external = false);

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