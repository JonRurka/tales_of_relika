#pragma once

#include "game_engine.h"

class Opaque_Structure_Chunk_Material : public Material {
public:

	Opaque_Structure_Chunk_Material();

	Material::Shared Copy() override 
	{ 
		auto mat = std::make_shared<Opaque_Structure_Chunk_Material>();
		mat->Set_Shader(Shader::Get_Shader("opaque_structure_chunk_material"));
		return mat;
	}

private:



};