#pragma once

#include <stdint.h>
#include <string>
#include <thread>
#include <functional>
#include <unordered_map>
#include <queue>

#include "OpCodes.h"
#include "Data.h"

#define SERVER_TCP_PORT 11010
#define SERVER_UDP_PORT 11011

#define MAX_UDP_SIZE UINT16_MAX


typedef void(*OnConnectActionPtr)(void*, bool);
typedef void(*CommandActionPtr)(void*, Data);

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

class NetClient {
public:

    enum class HostType
    {
        Local,
        Remote
    };

    NetClient();
    NetClient(std::string name, HostType connectionType, std::string remoteHost);
    ~NetClient();

    void AddCommand(OpCodes::Client cmd, CommandActionPtr callback, void* obj);

    bool Connected() {
        return m_tcp_socket_connected;
    }

    void SetOnConnectSuccess(OnConnectActionPtr cb, void* p) {
        OnConnect_delegate = cb;
        OnConnect_obj = p;
        //OnConnectSuccess_delegate = func_ptr;
        //OnConnectSuccess_delegate = (*static_cast<std::function<void()>*>(func_ptr));
    }

    void Connect();

    void Start_TCP();

    void Start_UDP(uint16_t port);

    void Process();

    bool Send(OpCodes::Server command, std::string data, Protocal type = Protocal_Tcp);

    bool Send(OpCodes::Server, std::vector<uint8_t> data, Protocal type = Protocal_Tcp);

    bool Send(std::vector<uint8_t> data, Protocal type = Protocal_Tcp);

    void Stop();

    void Disconnect(bool sendClose);

private:

    struct NetCommand {
        void* Obj_Ptr;
        CommandActionPtr Callback;
    };

    std::string m_name;
    std::string m_host;
    HostType m_hostType;
    uint16_t m_server_port_tcp;
    uint16_t m_server_port_udp;
    uint16_t m_client_port_udp;

    std::thread m_thread_recv_tcp;
    std::thread m_thread_recv_udp;
    std::thread m_thread_send;

    bool m_udp_socket_connected;
    bool m_tcp_socket_connected;
    bool m_run;
    bool trigger_stop;
    bool m_udp_started;

    uint64_t last_sent_ping;
    uint64_t last_received_ping;

    uint16_t m_UDP_ID;

    boost::asio::io_context m_io_context;

    tcp::socket m_socket_tcp;
    tcp::resolver m_resolver_tcp;

    udp::socket m_socket_udp;
    udp::resolver m_resolver_udp;
    udp::endpoint m_server_endpoint_udp;
    

    std::queue<std::vector<uint8_t>> m_tcp_recv_queue;
    std::queue<std::vector<uint8_t>> m_udp_recv_queue;

    std::queue<std::vector<uint8_t>> m_tcp_send_queue;
    std::queue<std::vector<uint8_t>> m_udp_send_queue;

    OnConnectActionPtr OnConnect_delegate;
    void* OnConnect_obj;


    std::unordered_map<uint8_t, NetCommand> m_commands;

    void send_ping();

    void async_loops();

    void send_tcp_connect_message();

    static void read_loop_tcp(NetClient* obj_ptr);

    static void read_loop_udp(NetClient* obj_ptr);

    static void SendLoop(NetClient* obj_ptr);

    void read_socket_tcp();

    void read_socket_udp();

    void handle_sends();

    void add_received_packet(std::vector<uint8_t> data, Protocal type);

    void process_cmd(std::vector<uint8_t> data, Protocal type);

    void do_tcp_send(std::vector<uint8_t> data);

    void do_udp_send(std::vector<uint8_t> data);


    std::vector<uint8_t> InsertCommand(uint8_t command, std::string data);

    std::vector<uint8_t> InsertCommand(uint8_t command, std::vector<uint8_t> data);

    static uint64_t GetEpoch() {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        return duration.count();
    }

    static void SystemCmds(void* obj, Data data) {
        ((NetClient*)(obj))->SystemCmds_internal(data);
    }

    void SystemCmds_internal(Data data);

    inline static const std::string LOG_LOC{ "NET_CLIENT" };
};
