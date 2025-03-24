#include <iostream>

#include "game_imp.h"
#include "Resources.h"
#include "Game_Resources.h"

#include <LuaCpp.hpp>

using namespace LuaCpp;
using namespace LuaCpp::Registry;
using namespace LuaCpp::Engine;

int main()
{
	//printf("%s\n", (Resources::Get_Resources_Director()).c_str());
	//Resources res(Resources::LoadMode::Filesystem);
	
	//Game_Resources* res = new Game_Resources();

	Game game;
	return game.Run();
}