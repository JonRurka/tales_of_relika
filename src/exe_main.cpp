#include <iostream>

#include "game_imp.h"
#include "Resources.h"
#include "Game_Resources.h"
#include "Utilities.h"

#include <LuaCpp.hpp>

using namespace LuaCpp;
using namespace LuaCpp::Registry;
using namespace LuaCpp::Engine;


int main()
{
	//printf("Padded: %s\n", intToPaddedString(5, 3).c_str());


	/*std::string data_dir = Resources::Get_Data_Director();
	printf("data dir: %s\n", data_dir.c_str());
	std::string base64 = Utilities::Read_File_String(data_dir + "s000.dat");
	std::string decoded = Utilities::Decode_Base64(base64);
	Logger::Update();

	//printf("Base64: %s\n", base64.c_str());
	printf("Decoded: %s\n", decoded.c_str());*/
	Logger::Set_Direct(true);
	Game game;
	return game.Run();
}