#pragma once

#include "game_engine.h"

class Standard_Material : public Material {
public:

	Standard_Material();

	Material* Copy() override { return new Standard_Material(); }

private:



};