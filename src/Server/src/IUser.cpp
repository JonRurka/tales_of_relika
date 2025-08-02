#include "IUser.h"
#include "Network/SocketUser.h"

void IUser::Send(OpCodes::Client cmd, std::vector<uint8_t> message, Protocal type)
{
	if (Connected()) {
		m_socket_user->Send(cmd, message, type);
	}
}

void IUser::Send(OpCodes::Client cmd, std::string message, Protocal type)
{
	if (Connected()) {
		m_socket_user->Send(cmd, message, type);
	}
}

int IUser::UDP_Send_Q_Size()
{
	return m_socket_user->udp_connection_client->Send_Q_Size();
}
