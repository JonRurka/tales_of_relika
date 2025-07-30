#include <iostream>

#include "Logger.h"
#include "Server_Main.h"

#include "dynamic_compute.h"
#include "Utilities.h"

using namespace DynamicCompute::Compute;

int main()
{
	Logger::Set_Direct(true);

	IComputeProgram::FileType m_type = IComputeProgram::FileType::Text_Data;

	OpenCL_Device_Info m_device_cl = Utilities::Get_Recommended_Device();
	Logger::LogDebug("main", "Using OpenCL Compute Device: %s", m_device_cl.name);

	m_device_cl.enable_context_sharing = false;

	ComputeInterface::ControllerInfo m_controllerInfo{};
	m_controllerInfo.device = &m_device_cl;
	m_controllerInfo.platform = m_device_cl.platform;
	m_controllerInfo.SetProgramDir("");

	IComputeController* m_controller = ComputeInterface::GetComputeController(ComputeInterface::OpenCL, m_controllerInfo);


	return 0;


	Server_Main::Options options{};
	options.Type = Server_Main::Server_Type::Remote;
	options.Async = false;
	Server_Main* server = new Server_Main(options);
	server->Start();
}