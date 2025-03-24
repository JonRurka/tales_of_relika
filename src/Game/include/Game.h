#pragma once

#include "game_engine.h"

class Game : public Engine
{
public:

protected:

	void Init() override;

	void Update(float dt) override;

private:

	const std::string LOG_LOC {"GAME"};

	void init_shaders();
};