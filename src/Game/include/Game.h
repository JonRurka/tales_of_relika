#pragma once

#include "game_engine.h"

class Material_Types;

class Game : public Engine
{
public:

protected:

	void Init() override;

	void Update(float dt) override;

private:

	Material_Types* m_material_types{nullptr};

	void init_shaders();

	void init_block_types();

	inline static const std::string LOG_LOC{ "GAME" };
};