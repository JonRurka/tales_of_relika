#pragma once

#include "game_engine.h"

class Opaque_Chunk_Material : public Material {
public:

	Opaque_Chunk_Material();

	Material::Shared Copy() override 
	{ 
		auto shader = std::make_shared<Opaque_Chunk_Material>();
		shader->Set_Shader(Shader::Get_Shader("opaque_chunk_material"));
		return shader;
	}

private:



};