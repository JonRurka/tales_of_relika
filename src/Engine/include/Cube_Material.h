#pragma once


#include "Material.h"

class Cube_Material : public Material
{
public:

	Cube_Material(std::string name);

	Material* Copy() override { return new Cube_Material(Name()); };

	void Update(float dt) override;

	

private:
	

};

