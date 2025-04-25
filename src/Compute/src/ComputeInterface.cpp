#include "ComputeInterface.h"
#include "ComputeInterface_private.h"
#include "IComputeController.h"

using namespace DynamicCompute::Compute;

IComputeController* DynamicCompute::Compute::ComputeInterface::GetComputeController(Compute_SDK implementation, ControllerInfo info)
{
	//printf("SDK 1: %i\n", (int)implementation);
	return new IComputeController(ComputeInterface_private::GetComputeController((ComputeInterface_private::Compute_SDK)implementation, *((ComputeInterface_private::ControllerInfo*)&info)));
}

void DynamicCompute::Compute::ComputeInterface::DisposePlatform(Compute_SDK implementation)
{
	ComputeInterface_private::DisposePlatform((ComputeInterface_private::Compute_SDK)implementation);
}

std::vector<Platform> DynamicCompute::Compute::ComputeInterface::GetSupportedPlatforms_OpenCL()
{
	std::vector<Platform> ret;
	return ret;
}

std::vector<OpenCL_Device_Info> DynamicCompute::Compute::ComputeInterface::GetSupportedDevices_OpenCL(Platform pltfrm)
{
	std::vector<OpenCL_Device_Info> res;
	return res;
}

std::vector<Vulkan_Device_Info> DynamicCompute::Compute::ComputeInterface::GetSupportedDevices_Vulkan()
{
	return ComputeInterface_private::GetSupportedDevices_Vulkan();
}

std::vector<DirectX_Device_Info> DynamicCompute::Compute::ComputeInterface::GetSupportedDevices_DirectX()
{
	std::vector<DirectX_Device_Info> ret;
	return ret;
}
