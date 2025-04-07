#pragma once

#include "game_engine.h"

class Opaque_Chunk_Material : public Material {
public:

	Opaque_Chunk_Material();

	Material* Copy() override { return new Opaque_Chunk_Material(); }

private:



};