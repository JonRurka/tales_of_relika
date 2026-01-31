#pragma once

#include "Network/AsyncServer.h"

class IUser : public std::enable_shared_from_this<IUser> {
	friend class SocketUser;
private:
	
	bool m_is_connected;
	boost::shared_ptr<SocketUser> m_socket_user;

	virtual void Set_Socket_User(boost::shared_ptr<SocketUser> socket_user) {
		m_is_connected = true;
		m_socket_user = socket_user;
	}

public:
	virtual boost::shared_ptr<SocketUser> Socket_User() {
		return m_socket_user;
	}

	virtual void Disconnected() {
		m_is_connected = false;
		m_socket_user = nullptr;
	}

	virtual bool Connected() {
		if (m_socket_user == nullptr) {
			return false;
		}

		return m_is_connected;
	}

	virtual void Disconnect(const std::string& reason);
	virtual void Send(OpCodes::Client cmd, std::vector<uint8_t> message, Protocal type = Protocal_Tcp);
	virtual void Send(OpCodes::Client cmd, std::string message, Protocal type = Protocal_Tcp);

	int UDP_Send_Q_Size();

protected:

};
