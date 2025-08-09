#pragma once

#include "game_engine.h"

class Opaque_Structure_Chunk_Material : public Material {
public:

	Opaque_Structure_Chunk_Material();

	Material::Shared Copy() override { return std::make_shared<Opaque_Structure_Chunk_Material>(); }

private:



};