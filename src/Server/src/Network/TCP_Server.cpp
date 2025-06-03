#include "TCP_Server.h"
#include "AsyncServer.h"
#include "SocketUser.h"
#include "Logger.h"
#include "../Server_Main.h"

void tcp_server::start_accept()
{
	tcp_connection::pointer new_connection =
		tcp_connection::create(io_service_);

	Logger::Log(LOG_POS("start_accept"), "Waiting for connection....");

	acceptor_.async_accept(new_connection->socket(),
		boost::bind(&tcp_server::handle_accept, this, new_connection,
			boost::asio::placeholders::error));
}

void tcp_server::handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error)
{
	start_accept();

	if (!error)
	{
		Logger::Log(LOG_POS("handle_accept"), "Accepting new connection.");
		auto user = std::shared_ptr<SocketUser>(new SocketUser(
			async_server, 
			new_connection));
		user->HandleStartConnect();
	}
	else {
		Logger::Log(LOG_POS("handle_accept"), "New connection failed.");
	}

	Server_Main::SetMemoryUsageForThread("tcp_service");
}

void tcp_server::close()
{
	m_run = false;
	io_service_.stop();
	m_thread.join();
}

void tcp_server::RunService(tcp_server* svr)
{
	Logger::Log(LOG_POS("RunService"), "Begin TCP service Run");
	while (svr->m_run) {
		svr->io_service_.run();
	}
	Logger::Log(LOG_POS("RunService"), "End TCP service Run");
}
