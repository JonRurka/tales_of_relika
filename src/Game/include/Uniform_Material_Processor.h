#pragma once

#include "game_engine.h"
#include "Material_Processor.h"


class Uniform_Material_Processor : public Material_Processor {
public:

	std::string Name() override;

	int Get_Tile_Texture_Index(int tile_id, uint8_t orientation) override;


protected:

	void Init() override;


private:



};