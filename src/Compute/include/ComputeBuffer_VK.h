#pragma once

#include "InterfaceIncludes_private.h"

namespace DynamicCompute {
    namespace Compute {
        namespace VK {

            // forward declarations
            class ComputeBuffer;

            class ComputeBuffer_VK : public IComputeBuffer_private {
                friend class ComputeController_VK;

            public:
                int SetData(void* data);

                int GetData(void* outData);

                int SetData(void* data, int size);

                int GetData(void* outData, int size);

                int SetData(void* data, int DstStart, int size);

                int GetData(void* outData, int SrcStart, int size);

                int CopyTo(IComputeBuffer_private* other);

                int CopyTo(IComputeBuffer_private* other, int size);

                int CopyTo(IComputeBuffer_private* other, int srcStart, int dstStart, int size);

                size_t GetSize();

                void Dispose();

                void* Get_Native_Ptr();

                unsigned int External_Buffer();

                unsigned int External_Memory();

                // Non-interface methods:

                ComputeBuffer* GetBuffer() { return m_buffer; }

            private:
                ComputeBuffer_VK(ComputeBuffer* src)
                    : m_buffer(src) {}

                ComputeBuffer* m_buffer;
            };


        }
    }
}