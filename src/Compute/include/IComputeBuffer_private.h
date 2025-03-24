#pragma once

#include "ForwardDeclarations.h"

namespace DynamicCompute {
    namespace Compute {

        class IComputeBuffer_private {
        public:
            virtual int SetData(void* data) = 0;

            virtual int GetData(void* outData) = 0;

            virtual int SetData(void* data, int size) = 0;

            virtual int GetData(void* outData, int size) = 0;

            virtual int SetData(void* data, int DstStart, int size) = 0;

            virtual int GetData(void* outData, int SrcStart, int size) = 0;

            virtual int CopyTo(IComputeBuffer_private* other) = 0;

            virtual int CopyTo(IComputeBuffer_private* other, int size) = 0;

            virtual int CopyTo(IComputeBuffer_private* other, int srcStart, int dstStart, int size) = 0;

            virtual size_t GetSize() = 0;

            virtual void Dispose() = 0;

            virtual void* Get_Native_Ptr() = 0;
        };

    }
}