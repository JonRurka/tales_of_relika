//#include "ComputeEngine.h"
#include "OpenCL/ComputeEngine_OCL.h"

#include "CL_SDK/cl.h"
#include "CL_SDK/opencl.hpp"
#include "CL_SDK/Utils/Utils.h"

#define WAIT_IDLE 1

using namespace DynamicCompute::Compute::OCL;

//#pragma comment(lib, "OpenCL.lib")

cl_platform_id ComputeEngine::platform_id = 0;
cl_uint ComputeEngine::num_of_platforms = 0;
cl_context_properties ComputeEngine::properties[3] = {0};
cl_device_id ComputeEngine::device_ids[MAX_OCL_DEVICES] = { 0 };
cl_uint ComputeEngine::num_of_devices = 0;
std::string ComputeEngine::app_dir;

std::list<ComputeContext> ComputeEngine::mContexts;

bool ComputeEngine::mInitialized{ false };

namespace {
    cl_event g_wait_event{ NULL }; // TODO: This is stupid. Change this.
}

/*std::vector<ComputeEngine::Platform> ComputeEngine::GetSupportedPlatforms()
{
    std::vector<ComputeEngine::Platform> res;

    num_of_platforms = 0;

    cl_platform_id all_platforms[10];

    // retreives a list of platforms available
    if (clGetPlatformIDs(10, all_platforms, &num_of_platforms) != CL_SUCCESS)
    {
        return res;
    }

    const int INFO_SIZE = 1000;

    char Info[INFO_SIZE];
    size_t n_size = 0;
    for (int i = 0; i < num_of_platforms; i++)
    {
        ComputeEngine::Platform platform;
        ZeroMemory(&platform, sizeof(ComputeEngine));

        platform.platform = all_platforms[i];

        ZeroMemory(Info, INFO_SIZE);
        clGetPlatformInfo(all_platforms[i], CL_PLATFORM_NAME, INFO_SIZE, Info, &n_size);
        strcpy(platform.name, Info);
        platform.name_size = n_size;

        ZeroMemory(Info, INFO_SIZE);
        clGetPlatformInfo(all_platforms[i], CL_PLATFORM_VENDOR, INFO_SIZE, Info, &n_size);
        strcpy(platform.vendor, Info);
        platform.vendor_size = n_size;

        ZeroMemory(Info, INFO_SIZE);
        clGetPlatformInfo(all_platforms[i], CL_PLATFORM_VERSION, INFO_SIZE, Info, &n_size);
        strcpy(platform.version, Info);
        platform.version_size = n_size;

        res.push_back(platform);
    }

    return res;
}

std::vector<ComputeEngine::Device> ComputeEngine::GetSupportedDevices(Platform pltfrm)
{
    std::vector<Device> res;

    num_of_devices = 0;

    

    // try to get a supported GPU device
    //ComputeEngine::device_ids = new cl_device_id[num_of_devices];
    cl_int get_res = clGetDeviceIDs(pltfrm.platform, CL_DEVICE_TYPE_ALL, MAX_DEVICES, device_ids, &num_of_devices);
    if (get_res != CL_SUCCESS)
    {
        printf("Failed to get devices: %i\n", get_res);
        return res;
    }

    const int INFO_SIZE = 1000;

    char Info[INFO_SIZE];
    cl_device_type type;
    size_t n_size = 0;
    for (int i = 0; i < num_of_devices; i++)
    {
        // CL_DEVICE_MAX_WORK_ITEM_SIZES
        Device dev;
        ZeroMemory(&dev, sizeof(Device));

        dev.device = device_ids[i];

        ZeroMemory(Info, INFO_SIZE);
        clGetDeviceInfo(device_ids[i], CL_DEVICE_VENDOR, INFO_SIZE, Info, &n_size);
        strcpy(dev.vendor, Info);
        dev.vendor_size = n_size;

        ZeroMemory(Info, INFO_SIZE);
        clGetDeviceInfo(device_ids[i], CL_DEVICE_NAME, INFO_SIZE, Info, &n_size);
        strcpy(dev.name, Info);
        dev.name_size = n_size;

        cl_uint freq;
        clGetDeviceInfo(device_ids[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &freq, &n_size);
        dev.clock_frequency = freq;

        cl_uint nm_unts;
        clGetDeviceInfo(device_ids[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &nm_unts, &n_size);
        dev.num_compute_units = nm_unts;

        cl_int num_dim;
        clGetDeviceInfo(device_ids[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &num_dim, &n_size);
        
        size_t* sizes = new size_t[num_dim];
        clGetDeviceInfo(device_ids[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * num_dim, sizes, &n_size);
        dev.max_work_size = 1;
        for (int w = 0; w < 2; w++)
        {
            dev.max_work_size *= static_cast<unsigned int>(sizes[w]);
        }
        delete[] sizes;

        size_t work_g_size;
        clGetDeviceInfo(device_ids[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &work_g_size, &n_size);
        dev.group_size = work_g_size;
        
        clGetDeviceInfo(device_ids[i], CL_DEVICE_TYPE, sizeof(cl_device_type), &type, &n_size);
        dev.is_type_default = (type & CL_DEVICE_TYPE_DEFAULT) == CL_DEVICE_TYPE_DEFAULT;
        dev.is_type_CPU = (type & CL_DEVICE_TYPE_CPU) == CL_DEVICE_TYPE_CPU;
        dev.is_type_GPU = (type & CL_DEVICE_TYPE_GPU) == CL_DEVICE_TYPE_GPU;
        dev.is_type_Accelerator = (type & CL_DEVICE_TYPE_ACCELERATOR) == CL_DEVICE_TYPE_ACCELERATOR;
        
        res.push_back(dev);
    }
    
    return res;
}*/

int ComputeEngine::Init(Platform pltform, std::string dir)
{
   
    platform_id = (cl_platform_id)pltform.platform;
    printf("Init Platform ID: %X\n", platform_id);

   // context properties list - must be terminated with 0
   properties[0] = CL_CONTEXT_PLATFORM;
   properties[1] = (cl_context_properties)platform_id;
   properties[2] = 0;

   app_dir = dir;

   mInitialized = true;

   return 0;
}

ComputeContext* ComputeEngine::GetNewContext(OpenCL_Device_Info device) {
    if (!mInitialized) {
        return nullptr;
    }
    
    mContexts.emplace_back(ComputeContext(properties, device));
    auto& buf = mContexts.back();
    buf.mCanCallDispose = true;
    return &buf;
}

std::string ComputeEngine::Get_CL_Version()
{
    char platformInfo[1000];
    size_t size = 0;

    cl_int status = clGetPlatformInfo(ComputeEngine::GetPlatform(), CL_PLATFORM_VERSION, 1000, platformInfo, &size);
    

    platformInfo[size] = '\0';
    std::string res = platformInfo;

    return res;
}

void ComputeEngine::Dispose() {
    properties[0] = 0;
    properties[1] = 0;
    properties[2] = 0;

    for (auto& context : mContexts) {
        context.Dispose();
    }

    mContexts.clear();

    mInitialized = false;
}



ComputeBuffer* ComputeContext::GetBuffer(ComputeBuffer::Buffer_Type type, size_t size, bool external)
{
    cl_mem_flags flags = 0;
    cl_mem_flags flags_staging = 0;

    switch (type)
    {
    case ComputeBuffer::Buffer_Type::READ:
        flags_staging = CL_MEM_ALLOC_HOST_PTR | CL_MEM_HOST_WRITE_ONLY;
        flags = CL_MEM_HOST_NO_ACCESS | CL_MEM_READ_ONLY;
        break;

    case ComputeBuffer::Buffer_Type::Write:
        flags_staging = CL_MEM_ALLOC_HOST_PTR | CL_MEM_HOST_READ_ONLY;
        flags = CL_MEM_HOST_NO_ACCESS | CL_MEM_WRITE_ONLY;
        break;

    case ComputeBuffer::Buffer_Type::Read_Write:
        flags_staging = CL_MEM_ALLOC_HOST_PTR;
        flags = CL_MEM_READ_WRITE;
        break;

    default:
        return NULL;
    }

    mBuffers.emplace_back(ComputeBuffer(context, command_queue, numContexts, flags, flags_staging, size));
    auto& buf = mBuffers.back();
    //buf.mCanCallDispose = true;
    return &buf;
}

void ComputeContext::Dispose()
{
    if (mDestroyed || !mInitialized || !mCanCallDispose)
        return;

    for (const auto& [key, value] : programs) {
        value->Dispose();
    }

    for (auto& buff : mBuffers) {
        buff.Dispose();
    }

    for (int i = 0; i < numContexts; i++)
    {
        clReleaseCommandQueue(command_queue);
    }

    programs.clear();
    mBuffers.clear();

    clReleaseContext(context);

    mDestroyed = true;
    mInitialized = false;
}

ComputeContext::ComputeContext(cl_context_properties properties[3], OpenCL_Device_Info device)
{
    cl_int err;
    numContexts = 0;

    deviceID = (cl_device_id)device.cl_device;

    cl_ulong local_size;
    clGetDeviceInfo(deviceID, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &local_size, 0);

    cl_ulong const_size;
    clGetDeviceInfo(deviceID, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_ulong), &const_size, 0);

    //CL_DEVICE_MAX_COMPUTE_UNITS
    cl_ulong comp_units;
    clGetDeviceInfo(deviceID, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &comp_units, 0);
    
    //CL_DEVICE_MAX_WORK_ITEM_SIZES
    size_t work_items[1];
    clGetDeviceInfo(deviceID, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(work_items), work_items, 0);
     
    //CL_DEVICE_MAX_WORK_GROUP_SIZE
    size_t work_group_size;
    clGetDeviceInfo(deviceID, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &work_group_size, 0);

    char driver_version[100];
    memset(driver_version, 0, 100);
    clGetDeviceInfo(deviceID, CL_DRIVER_VERSION, sizeof(driver_version), &driver_version, NULL);
    printf("OpenCL Driver Version: %s\n", driver_version);
    printf("OpenCl Platform Version: %s\n", ComputeEngine::Get_CL_Version().c_str());
    printf("OpenCl max local memory: %i bytes\n", (int)local_size);
    printf("OpenCl max const memory: %i bytes\n", (int)const_size);

    printf("OpenCl max compute units: %i\n", (int)comp_units);
    printf("OpenCl max work item sizes: (%i, %i, %i)\n", (int)work_items[0], (int)work_items[1], (int)work_items[2]);
    printf("OpenCl max group size: %i\n", (int)work_group_size);

    context = clCreateContext(properties, 1, &deviceID, NULL, NULL, &err);

    command_queue = clCreateCommandQueue(context, deviceID, 0, &err);
    numContexts = 1;

    //printf("Creating Compute Context with device: %s\n", device.name);

    mInitialized = true;
}

ComputeProgram* ComputeContext::Add_Program(std::string name)
{
    ComputeProgram* program = new ComputeProgram(this, context, command_queue);
    programs[name] = program;
    return program;
}

ComputeProgram* ComputeContext::Add_Program_Source(std::string name, const char* source)
{
   ComputeProgram* program = new ComputeProgram(this, context, command_queue);
   int res = program->Set_Source(source);
   programs[name] = program;
   return program;
}

ComputeProgram* ComputeContext::Add_Program_SPIRV(std::string name, const void* binary, size_t length)
{
    ComputeProgram* program = new ComputeProgram(this, context, command_queue);
    int res = program->Set_Binary(binary, length);

    if (res != 0) {
        printf("Add_Program_SPIRV(): Failed to create program: %i\n", res);
    }
    else {
        printf("Add_Program_SPIRV(): Created program: %i\n", res);
    }

    programs[name] = program;
    return program;
}

ComputeProgram* ComputeContext::Add_Program_SPIRV_File(std::string name, std::string file_path)
{
    ComputeProgram* program = new ComputeProgram(this, context, command_queue);

    int res = program->Set_Binary_File(file_path);

    if (res != 0) {
        printf("Add_Program_SPIRV_File(): Failed to create program: %i\n", res);
    }
    else {
        printf("Add_Program_SPIRV_File(): Created program: %i\n", res);
    }

    programs[name] = program;
    return program;
}

ComputeProgram* ComputeContext::Programs(std::string name)
{
   return programs[name];
}

ComputeKernel* ComputeContext::GetKernel(std::string p_name, std::string name)
{
   return Programs(p_name)->GetKernel(name);
}



ComputeProgram::ComputeProgram(ComputeContext* context_obj, cl_context context, cl_command_queue queue)
{
    mContextObj = context_obj;
    m_context = context;
    command_queue = queue;
}

int ComputeProgram::Set_Source(const char* source)
{
   cl_int err = 0;
   //printf("%s\n", source);
   program = clCreateProgramWithSource(m_context, 1, (const char **)&source, NULL, &err);
   args += "-cl-std=CL3.0 ";
   mInitialized = true;
   return err;
}

int ComputeProgram::Set_Binary(const void* binary, size_t length)
{
    cl_int err;
#if CL_TARGET_OPENCL_VERSION >= 210
    program = clCreateProgramWithIL(m_context, binary, length, &err);
    //args += "-x spir -spir-std=1.4 ";

    //CL_INVALID_CONTEXT
#else
    cl_device_id dvc = mContextObj->Get_CL_Device_ID();
    program = clCreateProgramWithBinary(m_context, 1, &dvc, &length, (const unsigned char**)&binary, NULL, &err)
#endif
    if (err == CL_INVALID_OPERATION) {
        printf("CL_INVALID_OPERATION\n");
    }

    printf("clCreateProgramWithIL: res %i\n", err);
    mInitialized = true;
    return err;
}

int ComputeProgram::Set_Binary_File(std::string file_path)
{
    int res = -1;
    //open file
    std::ifstream infile(file_path, std::ios::binary);
    char* buffer;

    //get length of file
    infile.seekg(0, infile.end);
    size_t length = infile.tellg();
    infile.seekg(0, infile.beg);

    //read file
    if (length > 0) {

        buffer = new char[length];
        infile.read(buffer, length);

        //buffer.resize(length);
        //buffer.insert(buffer.begin(), std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>());
        //infile.read(&buffer[0], length);
        //const void* binary = static_cast<void*>(buffer.data());

        printf("ComputeProgram.Set_Binary_File: Reading binary file of length %i\n", (int)length);
        res =Set_Binary(buffer, length);
    }
    else
    {
        printf("ComputeProgram.Set_Binary_File: File has length of zero.");
        res = -2;
    }

    

    return res;
}

void ComputeProgram::AddConstant(std::string name, std::string value)
{
   if (value.empty())
   {
      args += "-D " + name + " ";
   }
   else 
   {
      args += "-D " + name + "=" + value + " ";
   }
}

int ComputeProgram::Build(char* errorStr, size_t e_size)
{
    for (std::string inc_dir : mIncludeDirs)
    {
        args += "-I \"" + inc_dir + "\" ";
    }
   cl_int build_res = clBuildProgram(program, 0, NULL, args.c_str(), NULL, NULL);

   //printf("clBuildProgram: res %i\n", build_res);

   size_t ret_e_size;
   int res = clGetProgramBuildInfo(program, mContextObj->Get_CL_Device_ID(), CL_PROGRAM_BUILD_LOG, e_size, errorStr, &ret_e_size);

   if (build_res != 0) {
       printf("### COMPILATION FAILED: ###\n");
       printf("Build error: %s\n", errorStr);
       printf("###########################\\n");
   }

   return build_res;
}

ComputeKernel* ComputeProgram::GetKernel(std::string k_name)
{
   if (kernels.count(k_name) > 0)
   {
      return kernels[k_name];
   }

   ComputeKernel* new_kern = new ComputeKernel(this, (char*)k_name.c_str(), command_queue, program);
   kernels[k_name] = new_kern;
   return new_kern;
}

void ComputeProgram::Dispose()
{
    if (mDestroyed || !mInitialized || !mCanCallDispose)
        return;

    for (const auto& [key, value] : kernels) {
        value->Dispose();
    }

    clReleaseProgram(program);

    mDestroyed = true;
    mInitialized = false;
}



ComputeKernel::ComputeKernel(ComputeProgram* program_obj, char* name, cl_command_queue queue, cl_program program)
{
   cl_int err;
   //numKernels = numPrograms;
   m_program = program;
   command_queue = queue;
   mProgramObj = program_obj;

   //kernels = new cl_kernel[numKernels];
   //command_queue = new cl_command_queue[numKernels];
   
   //printf("ComputeKernel(): Create kernel %s\n", name);
   kernel = clCreateKernel(m_program, name, &err);

   if (err != 0)
   {
       printf("ComputeKernel(): Failed to create kernel: %i\n", err);

   }

   status = err;
   mInitialized = true;
}

int ComputeKernel::SetBuffer(ComputeBuffer* buffer, int arg)
{
    int res = 0;
    res = clSetKernelArg(kernel, (arg * 1) + 0, sizeof(cl_mem), (void*)buffer->Get_CL_Mem());
    //res = clSetKernelArg(kernel, (arg * 2) + 1, buffer->GetSize(), NULL);
    return res;
}

int ComputeKernel::Execute(cl_uint work_dim, size_t* global_work_size)
{
    /*size_t local_world_size[3] = {
        4, 4, 4
    };*/


    cl_event finished_event;
    int num_wait_events = (g_wait_event == NULL) ? 0 : 1;
    cl_event* wait_event_ptr = (g_wait_event == NULL) ? NULL : &g_wait_event;

    int res = clEnqueueNDRangeKernel(command_queue, kernel, work_dim, NULL, global_work_size, NULL, num_wait_events, wait_event_ptr, &finished_event);
    g_wait_event = finished_event;
    
    if (res != CL_SUCCESS)
    {
        printf("ComputeKernel.Execute: Failed to enqueue Kernel: %i\n", res);
    }

    // printf("clFinish.\n");

    if (WAIT_IDLE)
        clWaitForEvents(1, &g_wait_event);

    //res = clFinish(command_queue);
    //printf("Finished clFinish.\n");

    if (res != 0)
    {
        printf("ComputeKernel.Execute: clFinish failed after kernel enqueue: %i\n", res);
    }

    return res;
}

void ComputeKernel::Dispose()
{
    if (mDestroyed || !mInitialized || !mCanCallDispose)
        return;

    clReleaseKernel(kernel);

    mDestroyed = true;
    mInitialized = false;
}



ComputeBuffer::ComputeBuffer(cl_context contexts, cl_command_queue queue, int numContext, cl_mem_flags type, cl_mem_flags type_staging, size_t length)
{
   num = numContext;
   //buffer = new cl_mem[numContext];
   //command_queue = new cl_command_queue[numContext];
   mSize = length;

   context = contexts;
   command_queue = queue;

   cl_int err;


   buffer = clCreateBuffer(context, type, length, NULL, &err);
   buffer_staging = clCreateBuffer(context, type_staging, length, NULL, &err);

   mInitialized = true;
}

int ComputeBuffer::SetData(void* data)
{
    cl_event finished_event;
    int num_wait_events = (g_wait_event == NULL) ? 0 : 1;
    cl_event* wait_event_ptr = (g_wait_event == NULL) ? NULL : &g_wait_event;

    cl_int res = clEnqueueWriteBuffer(command_queue, buffer_staging, CL_TRUE, 0, mSize, data, num_wait_events, wait_event_ptr, &finished_event);
    g_wait_event = finished_event;

    if (res != 0)
    {
        return res;
    }

    wait_event_ptr = &g_wait_event;
    res = clEnqueueCopyBuffer(command_queue, buffer_staging, buffer, 0, 0, mSize, 1, wait_event_ptr, &finished_event);
    g_wait_event = finished_event;

    return res;
}

int ComputeBuffer::GetData(void* outData)
{
    cl_event finished_event;
    int num_wait_events = (g_wait_event == NULL) ? 0 : 1;
    cl_event* wait_event_ptr = (g_wait_event == NULL) ? NULL : &g_wait_event;

    cl_int res = clEnqueueCopyBuffer(command_queue, buffer, buffer_staging, 0, 0, mSize, num_wait_events, wait_event_ptr, &finished_event);

    if (res != 0)
    {
        return res;
    }

    wait_event_ptr = &g_wait_event;
    res = clEnqueueReadBuffer(command_queue, buffer_staging, CL_TRUE, 0, mSize, outData, 1, wait_event_ptr, &finished_event);
    g_wait_event = finished_event;
    //clWaitForEvents(1, &g_wait_event);
    return res;
}

int ComputeBuffer::SetData(void* data, int size) 
{
    cl_event finished_event;
    int num_wait_events = (g_wait_event == NULL) ? 0 : 1;
    cl_event* wait_event_ptr = (g_wait_event == NULL) ? NULL : &g_wait_event;

    cl_int res = clEnqueueWriteBuffer(command_queue, buffer_staging, CL_TRUE, 0, size, data, num_wait_events, wait_event_ptr, &finished_event);
    g_wait_event = finished_event;

    if (res != 0)
    {
        return res;
    }

    wait_event_ptr = &g_wait_event;
    res = clEnqueueCopyBuffer(command_queue, buffer_staging, buffer, 0, 0, size, 1, wait_event_ptr, &finished_event);
    g_wait_event = finished_event;

    return res;
}

int ComputeBuffer::GetData(void* outData, int size) 
{
    cl_event finished_event;
    int num_wait_events = (g_wait_event == NULL) ? 0 : 1;
    cl_event* wait_event_ptr = (g_wait_event == NULL) ? NULL : &g_wait_event;

    cl_int res = clEnqueueCopyBuffer(command_queue, buffer, buffer_staging, 0, 0, size, num_wait_events, wait_event_ptr, &finished_event);
    g_wait_event = finished_event;

    if (res != 0)
    {
        return res;
    }

    wait_event_ptr = &g_wait_event;
    res = clEnqueueReadBuffer(command_queue, buffer_staging, CL_TRUE, 0, size, outData, 1, wait_event_ptr, &finished_event);
    g_wait_event = finished_event;
    //clWaitForEvents(1, &g_wait_event);
    return res;
}

int ComputeBuffer::SetData(void* data, int DstStart, int size) 
{
    cl_event finished_event;
    int num_wait_events = (g_wait_event == NULL) ? 0 : 1;
    cl_event* wait_event_ptr = (g_wait_event == NULL) ? NULL : &g_wait_event;

    cl_int res = clEnqueueWriteBuffer(command_queue, buffer_staging, CL_TRUE, DstStart, size, data, num_wait_events, wait_event_ptr, &finished_event);
    g_wait_event = finished_event;

    if (res != 0)
    {
        return res;
    }

    wait_event_ptr = &g_wait_event;
    res = clEnqueueCopyBuffer(command_queue, buffer_staging, buffer, DstStart, DstStart, size, 1, wait_event_ptr, &finished_event);
    g_wait_event = finished_event;

    return res;
}

int ComputeBuffer::GetData(void* outData, int SrcStart, int size) 
{
    cl_event finished_event;
    int num_wait_events = (g_wait_event == NULL) ? 0 : 1;
    cl_event* wait_event_ptr = (g_wait_event == NULL) ? NULL : &g_wait_event;

    cl_int res = clEnqueueCopyBuffer(command_queue, buffer, buffer_staging, SrcStart, SrcStart, size, num_wait_events, wait_event_ptr, &finished_event);
    g_wait_event = finished_event;

    if (res != 0)
    {
        return res;
    }

    wait_event_ptr = &g_wait_event;
    res = clEnqueueReadBuffer(command_queue, buffer_staging, CL_TRUE, SrcStart, size, outData, 1, wait_event_ptr, &finished_event);
    g_wait_event = finished_event;
    //clWaitForEvents(1, &g_wait_event);
    return res;
}

int ComputeBuffer::CopyTo(ComputeBuffer* other) 
{
    cl_event finished_event;
    int num_wait_events = (g_wait_event == NULL) ? 0 : 1;
    cl_event* wait_event_ptr = (g_wait_event == NULL) ? NULL : &g_wait_event;


    int src_size = mSize;
    int dst_size = other->mSize;
    int size = std::min(src_size, dst_size);
    cl_int res = clEnqueueCopyBuffer(command_queue, buffer, other->buffer, 0, 0, size, num_wait_events, wait_event_ptr, &finished_event);
    g_wait_event = finished_event;
    return res;
}

int ComputeBuffer::CopyTo(ComputeBuffer* other, int size) 
{
    cl_event finished_event;
    int num_wait_events = (g_wait_event == NULL) ? 0 : 1;
    cl_event* wait_event_ptr = (g_wait_event == NULL) ? NULL : &g_wait_event;

    cl_int res = clEnqueueCopyBuffer(command_queue, buffer, other->buffer, 0, 0, size, num_wait_events, wait_event_ptr, &finished_event);
    g_wait_event = finished_event;
    return res;
}

int ComputeBuffer::CopyTo(ComputeBuffer* other, int srcStart, int dstStart, int size) 
{
    cl_event finished_event;
    int num_wait_events = (g_wait_event == NULL) ? 0 : 1;
    cl_event* wait_event_ptr = (g_wait_event == NULL) ? NULL : &g_wait_event;

    cl_int res = clEnqueueCopyBuffer(command_queue, buffer, other->buffer, srcStart, dstStart, size, num_wait_events, wait_event_ptr, &finished_event);
    g_wait_event = finished_event;
    return res;
}

void ComputeBuffer::Dispose()
{
    if (mDestroyed || !mInitialized || !mCanCallDispose)
        return;

    clReleaseMemObject(buffer);
    clReleaseMemObject(buffer_staging);

    mDestroyed = true;
    mInitialized = false;
}
