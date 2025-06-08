#include "LuaEngine.h"

#include "Resources.h"
#include "Utilities.h"

#include <filesystem>

#define SERVER_TYPE "server"
#define CLIENT_TYPE "client"

LuaEngine::LuaEngine()
{


}

LuaEngine::~LuaEngine()
{


}

namespace {
	std::string remove_extension(const std::string& filename) {
		return std::filesystem::path(filename).stem().string();
	}
}



void LuaEngine::Load_Client_Scripts()
{
	load_scripts(CLIENT_TYPE);
}

void LuaEngine::Load_Server_Scripts()
{
	load_scripts(SERVER_TYPE);
}

sol::state LuaEngine::NewServerState()
{
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::math);
	load_server_functions(lua);
	return lua;
}

std::string LuaEngine::Get_Script(std::string name)
{
	return m_scripts[name];
}

void LuaEngine::Set_Global_String(std::string key, std::string val)
{
	m_engine_mutex.lock();
	m_global_strings[key] = val;
	m_engine_mutex.unlock();
}

std::string LuaEngine::Get_Global_String(std::string key)
{
	m_engine_mutex.lock();
	std::string val = m_global_strings[key];
	m_engine_mutex.unlock();
	return val;
}

void LuaEngine::Set_Global_Number(std::string key, int val)
{
	m_engine_mutex.lock();
	m_global_int[key] = val;
	m_engine_mutex.unlock();
}

int LuaEngine::Get_Global_Number(std::string key)
{
	m_engine_mutex.lock();
	int val = m_global_int[key];
	m_engine_mutex.unlock();
	return val;
}

void LuaEngine::Set_Global_Decimal(std::string key, double val)
{
	m_engine_mutex.lock();
	m_global_Decimal[key] = val;
	m_engine_mutex.unlock();
}

double LuaEngine::Get_Global_Decimal(std::string key)
{
	m_engine_mutex.lock();
	double val = m_global_Decimal[key];
	m_engine_mutex.unlock();
	return val;
}



void LuaEngine::load_server_functions(sol::state& lua)
{
	lua.set_function("Set_String", &LuaEngine::Set_Global_String, this);
	lua.set_function("Get_String", &LuaEngine::Get_Global_String, this);
	lua.set_function("Set_Integer", &LuaEngine::Set_Global_Number, this);
	lua.set_function("Get_Integer", &LuaEngine::Get_Global_Number, this);
	lua.set_function("Set_Decimal", &LuaEngine::Set_Global_Decimal, this);
	lua.set_function("Get_Decimal", &LuaEngine::Get_Global_Decimal, this);



}

void LuaEngine::load_scripts(std::string type)
{
	std::vector<std::string> lua_resources = Resources::Get_Data_Resource_List(".lua");

	for (const auto& lua_file : lua_resources) {
		std::string script_name = Utilities::Get_Resource_Filename(lua_file);
		std::vector<std::string> name_parts = Utilities::Split(script_name, "_");

		std::string script_type = name_parts[0];
		std::string script_component_name = remove_extension(name_parts[1]);

		if (type != script_type) {
			continue;
		}

		std::string lua_content = Resources::Get_Data_File_String(lua_file);

		m_scripts[script_component_name] = lua_content;
	}
}






