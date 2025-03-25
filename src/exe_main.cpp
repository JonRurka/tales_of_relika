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
	Game game;
	return game.Run();
}