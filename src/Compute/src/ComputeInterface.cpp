#include "ComputeInterface.h"
#include "ComputeInterface_private.h"
#include "IComputeController.h"

using namespace DynamicCompute::Compute;

IComputeController* DynamicCompute::Compute::ComputeInterface::GetComputeController(Compute_SDK implementation, ControllerInfo info)
{
	//printf("SDK 1: %i\n", (int)implementation);
	return new IComputeController(ComputeInterface_private::GetComputeController((ComputeInterface_private::Compute_SDK)implementation, *((ComputeInterface_private::ControllerInfo*)&info)));
}

void ComputeInterface::DisposePlatform(Compute_SDK implementation)
{
	ComputeInterface_private::DisposePlatform((ComputeInterface_private::Compute_SDK)implementation);
}

std::vector<Platform> ComputeInterface::GetSupportedPlatforms_OpenCL()
{
	return ComputeInterface_private::GetSupportedPlatforms_OpenCL();
}

std::vector<OpenCL_Device_Info> ComputeInterface::GetSupportedDevices_OpenCL(Platform pltfrm)
{
	return ComputeInterface_private::GetSupportedDevices_OpenCL(pltfrm);
}

std::vector<Vulkan_Device_Info> ComputeInterface::GetSupportedDevices_Vulkan()
{
	return ComputeInterface_private::GetSupportedDevices_Vulkan();
}

std::vector<DirectX_Device_Info> ComputeInterface::GetSupportedDevices_DirectX()
{
	std::vector<DirectX_Device_Info> ret;
	return ret;
}
