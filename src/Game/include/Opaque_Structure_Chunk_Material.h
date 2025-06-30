#pragma once

#include "game_engine.h"

class Opaque_Structure_Chunk_Material : public Material {
public:

	Opaque_Structure_Chunk_Material();

	Material* Copy() override { return new Opaque_Structure_Chunk_Material(); }

private:



};