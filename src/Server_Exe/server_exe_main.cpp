#include <iostream>

#include "Logger.h"
#include "Server_Main.h"

int main()
{
	Server_Main::Options options{};
	options.Type = Server_Main::Server_Type::Remote;
	options.Async = false;
	Server_Main* server = new Server_Main(options);
	server->Start();
}