#pragma once

#include "PlatformStructures_private.h"

#include "OCL_forwardDeclarations.h"
//#include "CL/cl.h"
#include "CL/cl.h"
#include "CL/opencl.hpp"

// TODO: Clean up this file and the OpenCL implementation
// to match the other SDK platforms.

namespace DynamicCompute {
    namespace Compute {
        namespace OCL {

            class ComputeContext;
            class ComputeEngine;
            class ComputeKernel;
            class ComputeBuffer;
            class ComputeProgram;

            class ComputeBuffer
            {
            private:
                cl_context context;
                cl_mem buffer;
                cl_mem buffer_staging;
                cl_command_queue command_queue;
                int num;
                size_t mSize;

                cl_GLuint gl_buff;
                cl_mem cl_gl_buff;

                bool mInitialized{ false };
                bool mDestroyed{ false };
                bool mCanCallDispose{ true };
                bool mIsExternal{ false };

                inline static const std::string LOG_LOC{ "ComputeBuffer" };

            public:
                enum class Buffer_Type
                {
                    READ = 0,
                    Write = 1,
                    Read_Write = 2
                };


                ComputeBuffer(cl_context context, cl_command_queue queue, int numContext, cl_mem_flags type, cl_mem_flags type_staging, size_t length, bool external);

                cl_mem* Get_CL_Mem()
                {
                    return &buffer;
                }

                unsigned long long int External_GL_Buffer() { return gl_buff; }
                unsigned long long int External_Memory() { return (unsigned long long int)cl_gl_buff; }
                bool Is_External() { return mIsExternal; }

                int SetData(void* data);

                int GetData(void* outData);

                int SetData(void* data, int size);

                int GetData(void* outData, int size);

                int SetData(void* data, int DstStart, int size);

                int GetData(void* outData, int SrcStart, int size);

                int CopyTo(ComputeBuffer* other);

                int CopyTo(ComputeBuffer* other, int size);

                int CopyTo(ComputeBuffer* other, int srcStart, int dstStart, int size);

                void FlushExternal(int size);

                size_t GetSize() { return mSize; };

                void Dispose();
            };


            class ComputeKernel
            {
            private:

                cl_program m_program;
                cl_kernel kernel;
                cl_command_queue command_queue;
                int status;

                ComputeProgram* mProgramObj;

                std::vector<cl_mem> m_ext_buffers;

                //int numKernels;

                bool mInitialized{ false };
                bool mDestroyed{ false };
                bool mCanCallDispose{ true };

                inline static const std::string LOG_LOC{ "ComputeKernel" };

            public:
                ComputeKernel(ComputeProgram* program_obj, char* name, cl_command_queue command_queue, cl_program program);

                int GetStatus() { return status; }

                int SetBuffer(ComputeBuffer* buffer, int arg);

                int Execute(cl_uint work_dim, size_t* global_work_size);

                void Dispose();
            };

            class ComputeProgram {
            private:
                std::map<std::string, ComputeKernel*> kernels;
                cl_program program;
                cl_context m_context;
                ComputeContext* mContextObj;
                cl_command_queue command_queue;

                std::vector<std::string> mIncludeDirs;

                bool mInitialized{ false };
                bool mDestroyed{ false };
                bool mCanCallDispose{ true };

                std::string args;

                inline static const std::string LOG_LOC{ "ComputeProgram" };

            public:
                ComputeProgram(ComputeContext* context_obj, cl_context context, cl_command_queue queue);

                ComputeContext* GetContext() {
                    return mContextObj;
                }

                int Set_Source(const char* source);

                int Set_Binary(const void* binary, size_t length);

                int Set_Binary_File(std::string file_path);

                void AddConstant(std::string name, std::string value);

                void AddIncludeDirector(std::string dir)
                {
                    mIncludeDirs.push_back(dir);
                }

                int Build(char* errorStr, size_t e_size);

                ComputeKernel* GetKernel(std::string name);

                void Dispose();
            };

            class ComputeContext {
                friend class ComputeEngine;
            private:

                std::map<std::string, ComputeProgram*> programs;
                std::list<ComputeBuffer> mBuffers;

                cl_context context;
                cl_command_queue command_queue;
                cl_device_id deviceID;
                int numContexts;

                bool mInitialized{ false };
                bool mDestroyed{ false };
                bool mCanCallDispose{ true };

                inline static const std::string LOG_LOC{ "ComputeContext" };

            public:
                ComputeContext(cl_context_properties properties[3], OpenCL_Device_Info device);
                
                cl_device_id Get_CL_Device_ID() {
                    return deviceID;
                }

                //int SetProgram(const char* source);
                ComputeProgram* Add_Program(std::string name);
                ComputeProgram* Add_Program_Source(std::string name, const char* source);
                ComputeProgram* Add_Program_SPIRV(std::string name, const void* binary, size_t length);
                ComputeProgram* Add_Program_SPIRV_File(std::string name, std::string file_path);
                ComputeProgram* Programs(std::string name);

                ComputeKernel* GetKernel(std::string p_name, std::string name);
                ComputeBuffer* GetBuffer(ComputeBuffer::Buffer_Type type, size_t length, bool external);

                void Dispose();
            };


            class ComputeEngine
            {
            private:

                static cl_platform_id platform_id;
                static cl_uint num_of_platforms;
                static cl_context_properties properties[7];
                static cl_device_id device_ids[MAX_OCL_DEVICES];
                static cl_uint num_of_devices;

                static std::string app_dir;

                static std::list<ComputeContext> mContexts;

                static bool mInitialized;

                inline static const std::string LOG_LOC{ "ComputeEngine" };

            public:


                //static std::vector<Platform> GetSupportedPlatforms();
                //static std::vector<Device> GetSupportedDevices(Platform pltfrm);
                static int Init(Platform platform, std::string dir);
                static ComputeContext* GetNewContext(OpenCL_Device_Info device);

                static cl_platform_id GetPlatform()
                {
                    return platform_id;
                }

                static int GetNumDevices() {
                    return num_of_devices;
                }

                static std::string GetAppDir()
                {
                    return app_dir;
                }

                static bool IsInitialized() {
                    return mInitialized;
                }

                static std::string Get_CL_Version();

                static void Dispose();
            };

        }
    }
}