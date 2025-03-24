#pragma once

#include "InterfaceIncludes_private.h"
#include "PlatformStructures_private.h"

namespace DynamicCompute {
	namespace Compute {
		namespace VK {

			class ComputeContext;
			class ComputeEngine;
			class ComputeKernel;
			class ComputeBuffer;
			class ComputeProgram;

			class ComputeProgram_VK;

			class ComputeController_VK : public IComputeController_private {
				friend class ComputeInterface_private;

			public:
				void Init(Platform platform, void* device, std::string program_dir);

				IComputeProgram_private* AddProgram(IComputeProgram_private::ProgramInfo info);

				//IComputeProgram_private* AddProgram(std::string name, std::string kernel, std::vector<uint8_t> data);

				IComputeProgram_private* GetProgram(std::string name);

				IComputeBuffer_private* NewReadBuffer(size_t numElements, size_t stride);

				IComputeBuffer_private* NewWriteBuffer(size_t numElements, size_t stride);

				IComputeBuffer_private* NewReadWriteBuffer(size_t numElements, size_t stride);

				void Dispose();

				static void Close();

				// Non-interface methods:

				ComputeBuffer* NewBuffer(uint32_t type, size_t length);

				~ComputeController_VK();
				ComputeController_VK() {}
			private:
				

				

				static IComputeController_private* New();
				static void DisposePlatform();

				ComputeContext* m_context{ nullptr };
				std::string m_directory{ "" };

				std::map<std::string, ComputeProgram_VK*> m_programs;

				bool mDestroyed{ false };

				static std::vector<ComputeController_VK*> m_controllers;
			};


		}
	}
}