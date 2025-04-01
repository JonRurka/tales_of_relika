#pragma once

#include "ForwardDeclarations.h"
#include "IComputeBuffer_private.h"
#include "IComputeProgram_private.h"
#include "ComputeInterface_private.h"

namespace DynamicCompute {
	namespace Compute {

		class IComputeController_private {
		public:
			

			virtual void Init(Platform platform, void* device, std::string program_dir) = 0;

			virtual IComputeProgram_private* AddProgram(IComputeProgram_private::ProgramInfo info) = 0;

			virtual IComputeProgram_private* GetProgram(std::string name) = 0;

			virtual IComputeBuffer_private* NewReadBuffer(size_t numElements, size_t stride, bool external = false) = 0;

			virtual IComputeBuffer_private* NewWriteBuffer(size_t numElements, size_t stride, bool external = false) = 0;

			virtual IComputeBuffer_private* NewReadWriteBuffer(size_t numElements, size_t stride, bool external = false) = 0;

			virtual void Dispose() = 0;
		};

	}
}