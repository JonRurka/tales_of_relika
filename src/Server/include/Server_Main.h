#pragma once

#include "stdafx.h"
#include "Network/Data.h"

#define BOOST_TIMER_ENABLE_DEPRECATED
#include <boost/timer.hpp>

class CommandExecuter;
class Logger;
class AsyncServer;
class SocketUser;
class MatchManager;
class PlayerAuthenticator;
class Player;

class Server_Main {
public:

	enum class Server_Type {
		Remote,
		Local
	};

	struct Options {
		Server_Type Type;
		bool Async;
	};

private:

	struct QueueLengths {
		int UDP_SendQeue;
		int TCP_SendQeue;
		int Main_ReceiveQueue;
		int Async_ReceiveQueue;

	};

	static Server_Main* m_instance;

	Options m_options;

	std::string m_cmdArgs;
	std::string m_app_dir;
	bool m_running;

	CommandExecuter* m_com_executer;
	std::string m_curCommand;
	std::string m_executedCommand;

	std::thread m_loop_thread;

	int frameCounter;
	boost::timer timer;
	double lastTime;

	AsyncServer* m_net_server;

	MatchManager* m_match_manager;

	PlayerAuthenticator* m_authenticator;

	std::unordered_map<uint32_t, std::shared_ptr<Player>> m_players;

	std::map<std::string, uint64_t> m_memory_usage;
	std::mutex m_memory_lock;
	uint64_t m_last_memory_print_time;

	static QueueLengths m_queue_lengths;

public:

	

	static Server_Main* Instance() {
		return m_instance;
	}

	static CommandExecuter* GetInputProcessor()
	{
		return m_instance->m_com_executer;
	}

	static char sepChar() {
#ifdef WINDOWS_PLATFROM
		return '\\';
#else // linux
		return '/';
#endif
	}

	static std::string AppDirectory() {
		return m_instance->m_app_dir;
	}

	static std::string CmdArgs()
	{
		return m_instance->m_cmdArgs;
	}

	static bool Running()
	{
		return m_instance->m_running;
	}

	static double Elapsed() {
		return m_instance->timer.elapsed();
	}

	static void SetQueueLength_UDP_SendQeue(int num) {
		m_queue_lengths.UDP_SendQeue = num;
	}

	static void SetQueueLength_TCP_SendQeue(int num) {
		m_queue_lengths.TCP_SendQeue = num;
	}

	static void SetQueueLength_Main_ReceiveQueue(int num) {
		m_queue_lengths.Main_ReceiveQueue = num;
	}

	static void SetQueueLength_Async_ReceiveQueue(int num) {
		m_queue_lengths.Async_ReceiveQueue = num;
	}

	static uint64_t GetEpoch() {
		auto now = std::chrono::system_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
		return duration.count();
	}

	static uint64_t GetMemoryUsage();

	static void SetMemoryUsageForThread(std::string name) {
		SetMemoryUsageForThread(name, GetMemoryUsage());
	}

	static void SetMemoryUsageForThread(std::string name, uint64_t usage) {
		m_instance->m_memory_lock.lock();
		m_instance->m_memory_usage[name] = usage;
		m_instance->m_memory_lock.unlock();
	}

	void SetCurrentCommand(std::string command);

	void SetCommand(std::string command)
	{
		m_executedCommand = command;
	}

	void UserConnected(boost::shared_ptr<SocketUser> socket_user);

	void UserDisconnected(SocketUser* socket_user);

	void PlayerAuthenticated(std::shared_ptr<Player>, bool authorized);

	bool Has_Player(uint32_t p_id) {
		return m_players.find(p_id) != m_players.end();
	}

	void PrintQueueLengths();

	std::shared_ptr<Player> CreateFakePlayer(uint32_t id);

	Server_Main(char* args);

	Server_Main(Options options);


	void Start();

	void LoadSettings(std::string file);

	void Init();

	static void Async_Loop(Server_Main* srv) {
		srv->Loop();
	}
	void Loop();

	void Update(double dt);

	void Stop();

	void Dispose();

	static void UserIdentify_cb(void* obj, SocketUser& user, Data data) {
		m_instance->UserIdentify(user, data);
	}
	void UserIdentify(SocketUser& user, Data data);

	inline static const std::string LOG_LOC{ "SERVER_MAIN" };

};