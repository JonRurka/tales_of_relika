#include <iostream>

#include "Logger.h"
#include "Server_Main.h"

#include "dynamic_compute.h"
#include "Utilities.h"

using namespace DynamicCompute::Compute;

int main()
{
	Server_Main::Options options{};
	options.Type = Server_Main::Server_Type::Remote;
	options.Async = false;
	Server_Main* server = new Server_Main(options);
	server->Start();
}