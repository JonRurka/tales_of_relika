#include "NetClient.h"

#include "Logger.h"
#include "BufferUtils.h"


#define TIMEOUT_MS 10000
#define PING_SEND_MS 1000

namespace {
    uint8_t g_tcp_message[UINT16_MAX];
    uint8_t g_udp_message[UINT16_MAX + 2];
}

NetClient::NetClient() 
    : m_socket_tcp(m_io_context),
    m_socket_udp(m_io_context),
    m_resolver_tcp(m_io_context),
    m_resolver_udp(m_io_context)
{
}

NetClient::NetClient(std::string name, HostType connectionType, std::string remoteHost)
    : m_socket_tcp(m_io_context),
    m_socket_udp(m_io_context),
    m_resolver_tcp(m_io_context),
    m_resolver_udp(m_io_context)
{
    m_name = name;
    m_hostType = connectionType;
    int add_to_udp_port = 0;
    if (connectionType == HostType::Local)
    {
        m_host = "127.0.0.1";
        add_to_udp_port = 1;
    }
    else
    {
        m_host = remoteHost;
    }

    m_server_port_tcp = SERVER_TCP_PORT;
    m_server_port_udp = SERVER_UDP_PORT;
    m_client_port_udp = m_server_port_udp + add_to_udp_port;

    m_UDP_ID = 0;
    trigger_stop = false;

    AddCommand(OpCodes::Client::System_Reserved, SystemCmds, this);
}

NetClient::~NetClient()
{
    Stop();
}

void NetClient::AddCommand(OpCodes::Client cmd, CommandActionPtr callback, void* obj)
{
    if (!m_commands.contains((uint8_t)cmd)) {
        NetCommand command;
        command.Callback = callback;
        command.Obj_Ptr = obj;
        m_commands[(uint8_t)cmd] = command;
    }

}

void NetClient::Connect()
{
    Start_TCP();
}

void NetClient::Start_TCP()
{
    Logger::LogInfo(LOG_POS("Start_TCP"), "Starting net client '%s'", m_name.c_str());

    boost::system::error_code ec;
    boost::asio::connect(m_socket_tcp, m_resolver_tcp.resolve(m_host, std::to_string(m_server_port_tcp)), ec);
    
    if (ec) { // Error case
        Logger::LogError(LOG_POS("Start_TCP"), "Net client '%s' failed to connect!", m_name.c_str());
        m_tcp_socket_connected = false;
    }
    else {
        Logger::LogInfo(LOG_POS("Start_TCP"), "Net client '%s' connected.", m_name.c_str());
        m_tcp_socket_connected = true;
        async_loops();
    }


}

void NetClient::Start_UDP(uint16_t port)
{
    m_server_port_udp = port;

    boost::system::error_code ec;
    m_server_endpoint_udp = boost::asio::connect(m_socket_udp, m_resolver_udp.resolve(m_host, std::to_string(m_server_port_udp)), ec);

    if (ec) { // Error case
        Logger::LogError(LOG_POS("Start_TCP"), "Net client '%s' failed to connect!", m_name.c_str());
        m_udp_socket_connected = false;
    }
    else {
        Logger::LogInfo(LOG_POS("Start_TCP"), "Net client '%s' connected.", m_name.c_str());
        m_udp_socket_connected = true;
        m_thread_recv_udp = std::thread(&read_loop_udp, this);
    }

}

void NetClient::Process()
{
    
    if (trigger_stop) {
        Logger::Log(LOG_POS("Process"), "Calling net client stop from Proccess (trigger_stop)");
        trigger_stop = false;
        Stop();
    }

    if (!m_run) {
        return;
    }

    auto now = GetEpoch();

    
    if ((now - last_received_ping) >= TIMEOUT_MS) {
        //Logger::Log(LOG_POS("Process"), "Calling disconnect from Proccess (Timeout)");
        //Disconnect(false);
        //return;
    }
    

    if ((now - last_sent_ping) >= PING_SEND_MS) {
        send_ping();
    }

    std::vector<uint8_t> recv_buff;
    while (!m_tcp_recv_queue.empty()) {
        recv_buff = m_tcp_recv_queue.front();
        m_tcp_recv_queue.pop();

        process_cmd(recv_buff, Protocal_Tcp);
    }

    while (!m_udp_recv_queue.empty()) {
        recv_buff = m_udp_recv_queue.front();
        m_udp_recv_queue.pop();

        process_cmd(recv_buff, Protocal_Udp);
    }
}

bool NetClient::Send(OpCodes::Server command, std::string data, Protocal type)
{
    return Send(InsertCommand((uint8_t)command, data), type);
}

bool NetClient::Send(OpCodes::Server command, std::vector<uint8_t> data, Protocal type)
{
    return Send(InsertCommand((uint8_t)command, data), type);
}

bool NetClient::Send(std::vector<uint8_t> data, Protocal type)
{
    if (type == Protocal_Tcp) {
        m_tcp_send_queue.push(data);
    }
    else {
        m_udp_send_queue.push(data);
    }

    return true;
}

void NetClient::Stop()
{
    Logger::Log(LOG_POS("Stop"), "Net Client Stop");
    m_run = false;


    if (m_udp_socket_connected && m_socket_udp.is_open()) {
        m_socket_udp.close();
    }

    if (m_tcp_socket_connected && m_socket_tcp.is_open()) {
        //uint8 close_send[2] = {0xff, 0x02};
        //DoTcpSend(TArray<uint8>(close_send, 2));
        m_socket_tcp.close();
    }

    if (m_tcp_socket_connected) {

        m_thread_send.join();

        m_thread_recv_tcp.join();

        if (m_udp_socket_connected) {
            m_thread_recv_udp.join();
            m_udp_socket_connected = false;
        }

        m_tcp_socket_connected = false;
    }

}

void NetClient::Disconnect(bool sendClose)
{
    Logger::Log(LOG_POS("Disconnect"), "Net Client Disconnect");

    if (sendClose) {
        // TODO: send close
    }


    Logger::Log(LOG_POS("Disconnect"), "Calling net client stop from Disconnect");
    Stop();

}

void NetClient::send_ping()
{
    last_sent_ping = GetEpoch();
    Send(OpCodes::Server::System_Reserved, std::vector<uint8_t>({ 0x03, 0x01 }), Protocal_Tcp);
}

void NetClient::async_loops()
{
    send_tcp_connect_message();

    m_run = true;

    // spin up recv thread
    m_thread_recv_tcp = std::thread(&read_loop_tcp, this);
    //m_thread_recv_udp = std::thread(&ReadLoop_UDP, this);

    // Spin up send thread
    m_thread_send = std::thread(&SendLoop, this);

}

void NetClient::send_tcp_connect_message()
{
    last_sent_ping = GetEpoch();
    Send(OpCodes::Server::System_Reserved, std::vector<uint8_t>({ 0x01, 0x01 })); // send connect message.
}

void NetClient::read_loop_tcp(NetClient* obj_ptr)
{
    NetClient* clnt = (NetClient*)obj_ptr;
    while (clnt->m_run)
    {
        clnt->read_socket_tcp();
    }
}

void NetClient::read_loop_udp(NetClient* obj_ptr)
{
    NetClient* clnt = (NetClient*)obj_ptr;
    clnt->m_udp_started = true;
    while (clnt->m_run)
    {
        clnt->read_socket_udp();
    }
}

void NetClient::SendLoop(NetClient* obj_ptr)
{
    NetClient* clnt = (NetClient*)obj_ptr;
    while (clnt->m_run)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        clnt->handle_sends();
    }
}

void NetClient::read_socket_tcp()
{
    uint8_t length_buff[2];
    int32_t BytesRead = 0;

    boost::asio::read(m_socket_tcp, boost::asio::buffer(length_buff, 2));

    uint16_t size = *((uint16_t*)&length_buff);

    // TODO: garentee this won't fragment
    boost::asio::read(m_socket_tcp, boost::asio::buffer(g_tcp_message, size));

    std::vector<uint8_t> msg(g_tcp_message, g_tcp_message + size);

    add_received_packet(msg, Protocal_Tcp);
}

void NetClient::read_socket_udp()
{
    m_socket_udp.receive_from(boost::asio::buffer(g_udp_message), m_server_endpoint_udp);

    uint16_t size = *((uint16_t*)g_udp_message);

    uint8_t* data_ptr = &g_udp_message[2];
    std::vector<uint8_t> buffer(data_ptr, data_ptr + size);

    add_received_packet(buffer, Protocal_Udp);
}

void NetClient::handle_sends()
{
    std::vector<uint8_t> send_buff;
    while (!m_tcp_send_queue.empty()) {
        send_buff = m_tcp_send_queue.front();
        m_tcp_send_queue.pop();

        do_tcp_send(send_buff);
    }


    while (!m_udp_send_queue.empty()) {
        send_buff = m_udp_send_queue.front();
        m_udp_send_queue.pop();

        do_udp_send(send_buff);
    }
}

void NetClient::add_received_packet(std::vector<uint8_t> data, Protocal type)
{
    // TODO: Probably need to lock this.
    if (type == Protocal_Tcp) {
        m_tcp_recv_queue.push(data);
    }
    else {
        m_udp_recv_queue.push(data);
    }
}

void NetClient::process_cmd(std::vector<uint8_t> data, Protocal type)
{
    if (type == Protocal_Udp) {
        // Nothing to do currently with UDP ID.
        data = BufferUtils::RemoveFront(Remove_UDP_ID, data);
    }

    int8_t command = data[0];
    std::vector<uint8_t> dst = BufferUtils::RemoveFront(Remove_CMD, data);

    if (m_commands.contains(command)) {

        Data data_obj = Data(type, command, dst);

        NetCommand net_cmd = m_commands[command];
        net_cmd.Callback(net_cmd.Obj_Ptr, data_obj);
    }
    else {
        Logger::LogError(LOG_POS("process_cmd"), "Received invalid command % d", command);
    }
}

void NetClient::do_tcp_send(std::vector<uint8_t> data)
{
    if (!m_socket_tcp.is_open()) {
        return;
    }

    data = BufferUtils::AddLength(data);

    boost::asio::write(m_socket_tcp, boost::asio::buffer(data));
}

void NetClient::do_udp_send(std::vector<uint8_t> data)
{
    if (!m_socket_udp.is_open()) {
        return;
    }

    data = BufferUtils::Add_UDP_ID(m_UDP_ID, data);
    data = BufferUtils::AddLength(data);
    
    m_socket_udp.send_to(boost::asio::buffer(data), m_server_endpoint_udp);
}

std::vector<uint8_t> NetClient::InsertCommand(uint8_t command, std::string data)
{
	return InsertCommand(command, std::vector<uint8_t>(data.begin(), data.end())); // TODO: might need to change
}

std::vector<uint8_t> NetClient::InsertCommand(uint8_t command, std::vector<uint8_t> data)
{
    return BufferUtils::AddFirst(command, data);
}

void NetClient::SystemCmds_internal(Data data)
{
    uint8_t sub_command = data.Buffer[0];
    //UE_LOG(GameClient_Log, Display, TEXT("Received sub command %d"), sub_command);

    data.Buffer = BufferUtils::RemoveFront(Remove_CMD, data.Buffer);

    switch (sub_command) {
    case 0x01: // connected.
        if (data.Buffer[0] == 0x01) {
            m_UDP_ID = *((uint16_t*)(&data.Buffer.data()[1]));
            uint16_t udp_port = *((uint16_t*)(&data.Buffer.data()[3]));
            Logger::Log(LOG_POS("SystemCmds_internal"), "Received UDP ID '%d' on port %d", m_UDP_ID, udp_port);
            Start_UDP(udp_port);
            //uint32_t udp_port = (m_client_port_udp);
            //std::vector<uint8_t> port_arr = std::vector<uint8_t>((uint8_t*)&udp_port, 2);
            //Send(OpCodes::Server::System_Reserved, BufferUtils::AddFirst(0x04, port_arr));
            OnConnect_delegate(OnConnect_obj, true);
        }
        else {
            OnConnect_delegate(OnConnect_obj, false);
        }
        break;
    case 0x02:
        Logger::Log(LOG_POS("SystemCmds_internal"), "Calling disconnect from SystemCmds");
        Disconnect(false);
        break;
    case 0x03:
        last_received_ping = GetEpoch();
        //UE_LOG(GameClient_Log, Display, TEXT("Received pong"));
        break;
    }
}
