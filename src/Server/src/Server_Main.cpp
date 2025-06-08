#include "Server_Main.h"
#include "Logger.h"
#include "CommandExecuter.h"
#include "Network/AsyncServer.h"
#include "Network/SocketUser.h"
#include "HashHelper.h"
#include "Network/OpCodes.h"
#include "Network/PlayerAuthenticator.h"
#include "WorldController.h"
#include "World.h"
#include "IUser.h"
#include "Player.h"
#include "LuaEngine.h"
 
Server_Main* Server_Main::m_instance = nullptr;
Server_Main::QueueLengths Server_Main::m_queue_lengths{};

void Server_Main::UserConnected(boost::shared_ptr<SocketUser> socket_user)
{

	if (socket_user->Has_User() && !socket_user->GetUser().expired()) {
		std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(socket_user->GetUser().lock());

	}

	Logger::Log(LOG_POS("UserConnected"), "User '" + socket_user->SessionToken + "' has connected.");

	// Request basic user information
	//socket_user->Send(OpCodes::Client::Request_Identity, std::vector<uint8_t>({ 0x01 }));
}

void Server_Main::UserDisconnected(SocketUser* socket_user)
{
	//SocketUser* socket_user = (SocketUser*)socket_usr;

	Logger::Log(LOG_POS("UserDisconnected"), "Socket '" + socket_user->SessionToken + "' has disconnected.");

	if (!socket_user->GetUser().expired()) {
		Player* player = std::dynamic_pointer_cast<Player>(socket_user->GetUser().lock()).get();
		std::string username = player->Get_UserName();
		if (Has_Player(player->Get_UserID())) {
			m_players.erase(player->Get_UserID());
		}
		Logger::Log(LOG_POS("UserDisconnected"), "Player '" + username + "' removed.");
	}
	
}

void Server_Main::PlayerAuthenticated(std::shared_ptr<Player> player, bool authorized)
{
	if (authorized) {
		Logger::Log(LOG_POS("PlayerAuthenticated"), player->Get_UserName() + " authenticated successfully!");

		if (Has_Player(player->Get_UserID())) {
			Logger::Log(LOG_POS("PlayerAuthenticated"), player->Get_UserName() + " logged on again!");
			player->Socket_User()->Close(true);
			//delete player;
		}
		else {
			player->Socket_User()->Set_Authenticated(true);
			m_players[player->Get_UserID()] = player;
			PlayerJoined(player);
		}
	}
	else {
		Logger::Log(LOG_POS("PlayerAuthenticated"), player->Get_UserName() + " not authorized.");
		player->Socket_User()->Close(true);
		//delete player;
	}
}

void Server_Main::PlayerJoined(std::shared_ptr<Player> player)
{
	bool has_data = player->LoadPlayerData();
	if (has_data) {
		// load existing player
		uint64_t world_id = player->Player_Game_Data().CurrentWorldID;

		if (WorldController::GetInstance()->World_Exists(world_id)) {
			player->AssignPlayer(WorldController::GetInstance()->Get_World(world_id));
		}
	}
	else {
		// create new player profile.


	}


}

void Server_Main::PrintQueueLengths()
{
	std::string queue_lenths_str = "Network Queue Lengths: \n";

	queue_lenths_str += "\tUDP_SendQeue: " + std::to_string(m_queue_lengths.UDP_SendQeue) + "\n";
	queue_lenths_str += "\tTCP_SendQeue: " + std::to_string(m_queue_lengths.TCP_SendQeue) + "\n";
	queue_lenths_str += "\tMain_ReceiveQueue: " + std::to_string(m_queue_lengths.Main_ReceiveQueue) + "\n";
	queue_lenths_str += "\tAsync_ReceiveQueue: " + std::to_string(m_queue_lengths.Async_ReceiveQueue) + "\n";

	Logger::Log(LOG_POS("PrintQueueLengths"), queue_lenths_str);
}

std::shared_ptr<Player> Server_Main::CreateFakePlayer(uint32_t id)
{
	Player::PlayerIdentity fake_identity;
	fake_identity.UserName = "Fake_User_" + std::to_string(id);
	fake_identity.UserID = id;
	fake_identity.Distributor = 0;
	fake_identity.User_Distro_ID = "fake_distro_id";
	std::shared_ptr<Player> fake_player = std::shared_ptr<Player>(new Player());
	fake_player->SetIdentity(fake_identity);
	fake_player->Set_Location(glm::vec3(10, 5, 1.10));
	fake_player->Set_Rotation(glm::quat());
	return fake_player;
}

Server_Main::Server_Main(char* args)
{
	m_cmdArgs = args;

	m_instance = this;

	m_options.Type = Server_Type::Remote;

	frameCounter = 0;
	m_curCommand = "";
	m_executedCommand = "";


	char pBuf[256]{};
	size_t len = sizeof(pBuf);
#ifdef WINDOWS_PLATFROM
	int bytes = GetModuleFileName(NULL, pBuf, len);
#else // linux
	readlink("/proc/self/exe", pBuf, len);

#endif

	m_app_dir = pBuf;
	m_app_dir = m_app_dir.substr(0, m_app_dir.find_last_of('\\'));
}

Server_Main::Server_Main(Options options)
{
	m_instance = this;

	frameCounter = 0;
	m_curCommand = "";
	m_executedCommand = "";
	m_options = options;

	char pBuf[256]{};
	size_t len = sizeof(pBuf);
#ifdef WINDOWS_PLATFROM
	int bytes = GetModuleFileName(NULL, pBuf, len);
#else // linux
	readlink("/proc/self/exe", pBuf, len);

#endif

	m_app_dir = pBuf;
	m_app_dir = m_app_dir.substr(0, m_app_dir.find_last_of('\\'));
}

void Server_Main::Start()
{
	Init();
	if (m_options.Async) {
		m_loop_thread = std::thread(&Async_Loop, this);
	}
	else {
		Loop();
	}
}

void Server_Main::LoadSettings(std::string file)
{
}

void Server_Main::Init()
{
	m_running = true;

	m_com_executer = new CommandExecuter();
	m_com_executer->Run(false);

	//Logger::Log("Server Started!");

	m_lua_engine = new LuaEngine();

	m_authenticator = new PlayerAuthenticator(this);
	m_net_server = new AsyncServer(this);

	m_net_server->AddCommand(OpCodes::Server::Submit_Identity, Server_Main::UserIdentify_cb, this);
	//m_net_server->AddCommand(OpCodes::Server::Join_Match, Server_Main::JoinMatch_cb, this);

	m_last_memory_print_time = GetEpoch();

	Logger::Log(LOG_POS("Init"), "Server Initialized Successfully!");


	
	





}

void Server_Main::Loop()
{
	//Logger::Log(LOG_POS("Loop"), "Main Server Loop.");

	timer.restart();
	lastTime = timer.elapsed();
	while (m_running)
	{
		double curTime = timer.elapsed();
		double dt = curTime - lastTime;
		lastTime = curTime;

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		Update(dt);
		frameCounter++;
	}
}

void Server_Main::Update(double dt)
{
	if (!m_running)
		return;

	m_com_executer->Process();
	m_net_server->Update(dt);

	if (m_executedCommand != "")
	{


		// TODO: Add commands.

		m_executedCommand = "";
	}

	//Logger::Log("Memory Used: " + GetMemoryUsage());

	SetMemoryUsageForThread("main", GetMemoryUsage());

	auto now = GetEpoch();

	if ((now - m_last_memory_print_time) > 1000){
		/*m_memory_lock.lock();
		for (auto& [key, val] : m_memory_usage) {
			//Logger::Log("Thread " + key + ": " + std::to_string((val / 1000.0)) + "KB");
		}
		m_memory_lock.unlock();*/


		//PrintQueueLengths();

		glm::uvec2 udp_sends = m_net_server->Get_UDP_Sends();
		//Logger::Log(LOG_POS("Update"), "UDP packets sent: " + std::to_string(udp_sends.y) + " out of " + std::to_string(udp_sends.x));

		m_last_memory_print_time = now;
	}


	Logger::Update(); // TODO: decide if this should be called

}

void Server_Main::Stop()
{
	m_running = false;

	Dispose();
}

void Server_Main::Dispose()
{
}

uint64_t Server_Main::GetMemoryUsage()
{
	/*MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);

	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

	uint64_t physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
	uint64_t physMemUsedByMe = pmc.WorkingSetSize;*/

	return 0;// physMemUsedByMe;
}

std::shared_ptr<Player> Server_Main::GetPlayer(uint32_t id)
{
	if (!m_instance->Has_Player(id)) {
		throw std::exception("Requested player that does not exist!");
	}

	return m_instance->m_players[id];
}

void Server_Main::SetCurrentCommand(std::string command)
{
	m_curCommand = command;
}

void Server_Main::UserIdentify(SocketUser& user, Data data)
{
	Logger::LogDebug(LOG_POS("UserIdentify"), "Identifying user.");

	std::shared_ptr<Player> player = std::make_shared<Player>();
	user.SetUser(std::static_pointer_cast<IUser>(player));

	bool is_identified = player->SetIdentity(HashHelper::BytesToString(data.Buffer));

	if (is_identified) {
		m_authenticator->Authenticate(player);
	}
	else {
		user.Close(true);
		//delete player;
	}

	uint8_t res = is_identified ? 0x01 : 0x00;

	user.Send(OpCodes::Client::Identify_Result, std::vector<uint8_t>({ res }));
}


