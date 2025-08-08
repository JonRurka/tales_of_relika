#pragma once


#include "Material.h"

class Cube_Material : public Material
{
public:

	Cube_Material(std::string name);

	std::shared_ptr<Material> Copy() override { return std::make_shared<Cube_Material>(Name()); };

	void Update(float dt) override;

	

private:
	

};

