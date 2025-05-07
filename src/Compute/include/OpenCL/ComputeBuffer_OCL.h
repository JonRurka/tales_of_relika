#pragma once

#include "OCL_forwardDeclarations.h"
#include "InterfaceIncludes_private.h"
//#include "ComputeEngine.h"
#include "OpenCL/ComputeEngine_OCL.h"

namespace DynamicCompute {
    namespace Compute {
        namespace OCL {

            class ComputeBuffer_OCL : public IComputeBuffer_private{

                friend class ComputeController_OCL;
                
            public:
                int SetData(void* data) override;

                int GetData(void* outData) override;

                int SetData(void* data, int size) override;

                int GetData(void* outData, int size) override;

                int SetData(void* data, int DstStart, int size) override;

                int GetData(void* outData, int SrcStart, int size) override;

                int CopyTo(IComputeBuffer_private* other) override;

                int CopyTo(IComputeBuffer_private* other, int size) override;

                int CopyTo(IComputeBuffer_private* other, int srcStart, int dstStart, int size) override;

                void FlushExternal(int size);

                size_t GetSize() override;

                void Dispose() override;

                void* Get_Native_Ptr() override;

                unsigned int External_Buffer() override;

                unsigned int External_Memory() override;


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