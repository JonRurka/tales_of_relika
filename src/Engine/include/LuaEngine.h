#pragma once

#include <vector>
#include <unordered_map>
#include <mutex>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

class LuaEngine {
public:

	LuaEngine();
	~LuaEngine();

	void Load_Server_Scripts();

	void Load_Client_Scripts();

	sol::state NewServerState();

	std::string Get_Script(std::string name);

	void Set_Global_String(std::string key, std::string val);
	std::string Get_Global_String(std::string key);

	void Set_Global_Number(std::string key, int val);
	int Get_Global_Number(std::string key);

	void Set_Global_Decimal(std::string key, double val);
	double Get_Global_Decimal(std::string key);


private:

	void load_server_functions(sol::state& lua);

	std::unordered_map<std::string, std::string> m_scripts;


	std::unordered_map<std::string, std::string> m_global_strings;
	std::unordered_map<std::string, int> m_global_int;
	std::unordered_map<std::string, double> m_global_Decimal;

	std::mutex m_engine_mutex;

	void load_scripts(std::string type);

};