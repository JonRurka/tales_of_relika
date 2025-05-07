#include "ComputeInterface_private.h"

#include "ComputeController_OCL.h"
//#include "Compute_OCL/compute_test.h"
//#pragma comment(lib, "OpenCL.lib")

#include "ComputeController_VK.h"
#include "vulkan_utils.h"

//#include "Compute_Vulkan/vulkan_test.h"
//#include "Compute_Vulkan/vulkan_compute_test.h"

#ifdef WINDOWS_PLATFROM
//#include "Compute_DirectX/SystemClass_test.h"
//#include "Compute_DirectX/compute_test.h"
//#include "Compute_DirectX/ComputeController_DX.h"
//#include "Compute_DirectX/directX_utils.h"

#endif

using namespace DynamicCompute::Compute;
using namespace DynamicCompute::Compute::OCL;
using namespace DynamicCompute::Compute::VK;

#ifdef WINDOWS_PLATFROM
//using namespace DynamicCompute::Compute::DX;
#endif



cl_uint ComputeInterface_private::num_of_platforms = 0;
cl_uint ComputeInterface_private::num_of_devices = 0;
cl_device_id ComputeInterface_private::device_ids[MAX_OCL_DEVICES] = { 0 };

IComputeController_private* ComputeInterface_private::GetComputeController(ComputeInterface_private::Compute_SDK implementation, ControllerInfo info)
{
    //printf("SDK 2: %i\n", (int)implementation);
    //printf("getting controller\n");
	switch (implementation)
	{
	case ComputeInterface_private::Compute_SDK::OpenCL:
		return GetComputeController_OCL(info);

	case ComputeInterface_private::Compute_SDK::VULKAN:
		return GetComputeController_Vulkan(info);

#ifdef WINDOWS_PLATFROM
	//case ComputeInterface_private::Compute_SDK::DIRECTX:
		//return GetComputeController_DirectX(info);

    //case ComputeInterface_private::Compute_SDK::CUDA:
        //return GetComputeController_CUDA(info);
#endif

	default:
		// This should never happen.
        printf("Not controller platform defined\n");
		return nullptr;
	}
}

void ComputeInterface_private::DisposePlatform(Compute_SDK implementation)
{
    switch (implementation) {
    case Compute_SDK::VULKAN:
        ComputeController_VK::DisposePlatform();
        break;

#ifdef WINDOWS_PLATFROM
    //case Compute_SDK::DIRECTX:
    //    ComputeController_DX::DisposePlatform();
    //    break;
#endif
    }
}

IComputeController_private* ComputeInterface_private::GetComputeController_OCL(ControllerInfo info)
{
    //compute_test test;
    //test.Run(info.platform, info.device);

    //return nullptr;

    IComputeController_private* controller = ComputeController_OCL::New();

    controller->Init(info.platform, info.device, info.program_dir);

	return controller;
}

/*IComputeController_private* ComputeInterface_private::GetComputeController_CUDA(ControllerInfo info)
{
	return nullptr;
}*/

IComputeController_private* ComputeInterface_private::GetComputeController_Vulkan(ControllerInfo info)
{
    //Vulkan_compute_test test;
    //test.Run();

    //return nullptr;
    
    IComputeController_private* controller = ComputeController_VK::New();

    controller->Init(info.platform, info.device, info.program_dir);

    if (controller == nullptr) {
        printf("the controller is null\n");
    }
    else {
        //printf("the controller is not null\n");
    }

    return controller;
}

/*IComputeController_private* ComputeInterface_private::GetComputeController_DirectX(ControllerInfo info)
{
    IComputeController_private* controller = nullptr;

#ifdef WINDOWS_PLATFROM

    //printf("Running DirectX test app.\n");
    //compute_test test;
    //test.Run();

    controller = ComputeController_DX::New();

    controller->Init(info.platform, info.device, info.program_dir);

#endif

	return controller;
}*/


std::vector<Platform> ComputeInterface_private::GetSupportedPlatforms_OpenCL()
{
    std::vector<Platform> res;

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
        Platform platform{};

        platform.platform = all_platforms[i];
        printf("Platform ID: %llX\n", (long long)platform.platform);

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



std::vector<OpenCL_Device_Info> ComputeInterface_private::GetSupportedDevices_OpenCL(Platform pltfrm)
{
    std::vector<OpenCL_Device_Info> res;

    num_of_devices = 0;



    // try to get a supported GPU device
    //ComputeEngine::device_ids = new cl_device_id[num_of_devices];
    cl_int get_res = clGetDeviceIDs((cl_platform_id)pltfrm.platform, CL_DEVICE_TYPE_ALL, MAX_OCL_DEVICES, device_ids, &num_of_devices);
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
        //Device device{};
        OpenCL_Device_Info info{};

        info.cl_device = device_ids[i];
        info.platform = pltfrm;

        ZeroMemory(Info, INFO_SIZE);
        n_size = 0;
        clGetDeviceInfo(device_ids[i], CL_DEVICE_VENDOR, INFO_SIZE, Info, &n_size);
        strcpy(info.vendor, Info);
        info.vendor_size = n_size;

        ZeroMemory(Info, INFO_SIZE);
        n_size = 0;
        clGetDeviceInfo(device_ids[i], CL_DEVICE_NAME, INFO_SIZE, Info, &n_size);
        strcpy(info.name, Info);
        info.name_size = n_size;

        cl_uint freq;
        n_size = 0;
        clGetDeviceInfo(device_ids[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &freq, &n_size);
        info.clock_frequency = freq;

        cl_uint nm_unts;
        n_size = 0;
        clGetDeviceInfo(device_ids[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &nm_unts, &n_size);
        info.num_compute_units = nm_unts;

        cl_int num_dim;
        n_size = 0;
        clGetDeviceInfo(device_ids[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &num_dim, &n_size);

        size_t* sizes = new size_t[num_dim];
        n_size = 0;
        clGetDeviceInfo(device_ids[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * num_dim, sizes, &n_size);
        info.max_work_size = 1;
        for (int w = 0; w < 2; w++)
        {
            info.max_work_size *= static_cast<unsigned int>(sizes[w]);
        }
        delete[] sizes;

        size_t work_g_size;
        n_size = 0;
        clGetDeviceInfo(device_ids[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &work_g_size, &n_size);
        info.group_size = work_g_size;

        cl_device_local_mem_type local_mem_type;
        n_size = 0;
        clGetDeviceInfo(device_ids[i], CL_DEVICE_LOCAL_MEM_TYPE, sizeof(cl_device_local_mem_type), &local_mem_type, &n_size);
        //printf("Device Local Memory Type: %u\n", local_mem_type);

        cl_ulong local_mem_size;
        n_size = 0;
        clGetDeviceInfo(device_ids[i], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &local_mem_size, &n_size);
        //printf("Device local memory size: %llu\n", local_mem_size);
        info.local_memory_size = (unsigned int)local_mem_size;

        n_size = 0;
        clGetDeviceInfo(device_ids[i], CL_DEVICE_TYPE, sizeof(cl_device_type), &type, &n_size);
        info.is_type_default = (type & CL_DEVICE_TYPE_DEFAULT) == CL_DEVICE_TYPE_DEFAULT;
        info.is_type_CPU = (type & CL_DEVICE_TYPE_CPU) == CL_DEVICE_TYPE_CPU;
        info.is_type_GPU = (type & CL_DEVICE_TYPE_GPU) == CL_DEVICE_TYPE_GPU;
        info.is_type_Accelerator = (type & CL_DEVICE_TYPE_ACCELERATOR) == CL_DEVICE_TYPE_ACCELERATOR;

        //device.OpenCL_Info = info;

        res.push_back(info);
    }

    return res;
}



std::vector<Vulkan_Device_Info> ComputeInterface_private::GetSupportedDevices_Vulkan()
{
    std::vector<Vulkan_Device_Info> result;

    VkApplicationInfo appInfo = VK::Utilities::getApplicationInfo(
        "Get_Devices",
        VK_MAKE_VERSION(1, 0, 0),
        "Dynamics_IO",
        VK_MAKE_VERSION(1, 0, 0),
        VK_API_VERSION_1_3);


    std::vector<const char*> requiredExtensions;
    VkInstanceCreateInfo createInfo = VK::Utilities::getInstanceCreateInfo(appInfo, requiredExtensions);
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;

    VkResult res;
    VkInstance instance;
    res = vkCreateInstance(&createInfo, nullptr, &instance);

    if (res != VK_SUCCESS) {
        printf("ComputeInterface_private::GetSupportedDevices_Vulkan(): Error Initializing Vulkan Instance: %u\n", res);
        return result;
    }

    std::vector<VkPhysicalDevice> devices = VK::Utilities::EnumeratePhysicalDevices(instance);

    //printf("Total Devices: %li\n", devices.size());

    

    for (const auto& device : devices) {
        if (!VK_isDeviceSuitable(device)) {
            continue;
        }

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures{};
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        /*VkPhysicalDeviceProperties2 deviceProperties2{};
        deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
        vkGetPhysicalDeviceProperties2(device, &deviceProperties2);
        VkPhysicalDeviceProperties deviceProperties = deviceProperties2.properties;
        VkPhysicalDeviceIDProperties deviceIDProperties = *((VkPhysicalDeviceIDProperties*)deviceProperties2.pNext);*/
            
        //Device res_device{};
        Vulkan_Device_Info info{};

        memcpy(info.Name, deviceProperties.deviceName, 256);
        info.name_size = std::string(deviceProperties.deviceName).size();


        info.Device_ID = deviceProperties.deviceID;

        //memcpy(info.DeviceUUID, deviceIDProperties.deviceUUID, 16);

        switch (deviceProperties.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            info.Type = DeviceType::DEVICE_TYPE_OTHER;
            break;

        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            info.Type = DeviceType::DEVICE_TYPE_INTEGRATED_GPU;
            break;

        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            info.Type = DeviceType::DEVICE_TYPE_DISCRETE_GPU;
            break;

        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            info.Type = DeviceType::DEVICE_TYPE_VIRTUAL_GPU;
            break;

        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            info.Type = DeviceType::DEVICE_TYPE_CPU;
            break;
        }
            
        info.vk_device = device;
        //res_device.Vulkan_Info = info;
        result.push_back(info);
        
    }


    vkDestroyInstance(instance, nullptr);

    return result;
}

/*
std::vector<DirectX_Device_Info> ComputeInterface_private::GetSupportedDevices_DirectX()
{
    std::vector<DirectX_Device_Info> result;

#ifdef WINDOWS_PLATFROM

    std::vector<IDXGIAdapter*> adapters = DX::Utilities::GetAdapters();

    DXGI_ADAPTER_DESC2 desc;
    int adpt_ind = 0;
    for (auto& adpt : adapters) {
        adpt->GetDesc((DXGI_ADAPTER_DESC*)&desc);

        DirectX_Device_Info info{};

        std::wstring desc_wstr(desc.Description);
        std::string desc_str = std::string(desc_wstr.begin(), desc_wstr.end());
        memcpy(info.Name, desc_str.data(), desc_str.size());
        info.name_size = desc_str.size();

        info.AdapterIndex = adpt_ind;

        info.Device_ID = desc.DeviceId;

        // TODO: figure out how to determine type.
        // NOTE: Vulkan has the same DeviceIDs as
        // directx in some, most or all cases. If
        // they are the same, the device ID could
        // be used to poll vulkan to determine the
        // device type. Alternatively, and this
        // would be the better option, would be
        // to use the Device UUID, as soon as I can
        // figure out how to make that work in VK.
        info.Type = DeviceType::DEVICE_TYPE_OTHER;

        //Device device{};
        //device.DirectX_Info = info;

        result.push_back(info);

        adpt_ind++;
    }


    DX::Utilities::ReleaseAdapters(adapters);

#endif

    return result;
}
*/

bool ComputeInterface_private::VK_isDeviceSuitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    VK::Utilities::QueueFamilyIndices indices = VK::Utilities::findQueueFamilies(device);

    std::vector<const char*> deviceExtensions;// TODO: User-provided extensions.
    bool extensionsSupported = VK::Utilities::checkDeviceExtensionSupport(device, deviceExtensions);

    return indices.isComplete() && extensionsSupported;
}
