#pragma once

#include "std_includes.h"

namespace DynamicCompute {
    namespace Compute {

        class IComputeBuffer {
            friend class IComputeController_private;
            friend class IComputeProgram;
            friend class IComputeController;
        public:
            int SetData(void* data);

            int GetData(void* outData);

            int SetData(void* data, int size);

            int GetData(void* outData, int size);

            int SetData(void* data, int DstStart, int size);

            int GetData(void* outData, int SrcStart, int size);

            int CopyTo(IComputeBuffer* other);

            int CopyTo(IComputeBuffer* other, int size);

            int CopyTo(IComputeBuffer* other, int srcStart, int dstStart, int size);

            size_t GetSize();

            void Dispose();

            void* Get_Native_Ptr();

            unsigned int External_Buffer();

            unsigned int External_Memory();

        private:

            IComputeBuffer(IComputeBuffer_private* p) { p_inst = p; }

            IComputeBuffer_private* p_inst;
        };

    }
}