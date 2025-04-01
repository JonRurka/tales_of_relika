#pragma once

#include "std_includes.h"
#include "IComputeBuffer.h"
#include "IComputeProgram.h"

namespace DynamicCompute {
	namespace Compute {

		class IComputeController_private;

		class IComputeController {
			friend class ComputeInterface;
		public:

			void Init(Platform platform, void* device, std::string program_dir);
			
			IComputeProgram* AddProgram(IComputeProgram::ProgramInfo& info);

			//IComputeProgram* AddProgram(std::string name, std::string kernel, std::vector<uint8_t> data);
			
			IComputeProgram* GetProgram(std::string name);
			
			IComputeBuffer* NewReadBuffer(size_t numElements, size_t stride, bool external = false);
			
			IComputeBuffer* NewWriteBuffer(size_t numElements, size_t stride, bool external = false);
			
			IComputeBuffer* NewReadWriteBuffer(size_t numElements, size_t stride, bool external = false);
			
			void Dispose();

		private:

			IComputeController(IComputeController_private* p) { p_inst = p; }

			IComputeController_private* p_inst;

		};

	}
}