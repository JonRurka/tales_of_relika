#pragma once

#include "OCL_forwardDeclarations.h"
#include "InterfaceIncludes_private.h"
//#include "ComputeEngine.h"
#include "OpenCL/ComputeEngine.h"

namespace DynamicCompute {
    namespace Compute {
        namespace OCL {

            class ComputeBuffer_OCL : public IComputeBuffer_private{

                friend class ComputeController_OCL;
                
            public:
                int SetData(void* data);

                int GetData(void* outData);

                int SetData(void* data, int size);

                int GetData(void* outData, int size);

                int SetData(void* data, int DstStart, int size) { return 0; }

                int GetData(void* outData, int SrcStart, int size) { return 0; }

                int CopyTo(IComputeBuffer_private* other) { return 0; }

                int CopyTo(IComputeBuffer_private* other, int size) { return 0; }

                int CopyTo(IComputeBuffer_private* other, int srcStart, int dstStart, int size) { return 0; }

                size_t GetSize();

                void Dispose();

                void* Get_Native_Ptr();


                // Non-interface methods:

                ComputeBuffer* GetBuffer() { return m_buffer; }

            private:
                ComputeBuffer_OCL(ComputeBuffer* src)
                    : m_buffer(src){}

                ComputeBuffer* m_buffer;
            };

        }
    }
}