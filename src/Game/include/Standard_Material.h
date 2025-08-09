#pragma once

#include "game_engine.h"

class Standard_Material : public Material {
public:
	typedef std::shared_ptr<Standard_Material> Shared;
	typedef std::weak_ptr<Standard_Material> Weak;


	Standard_Material();

	Material::Shared Copy() override 
	{ 

		auto mat = std::make_shared<Standard_Material>();
		mat->Set_Shader(Shader::Get_Shader("standard"));
		return mat;
	}


private:



};