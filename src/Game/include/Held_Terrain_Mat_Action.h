#pragma once

#include "game_engine.h"

#include "Held_Item_Action.h"

class Held_Terrain_Mat_Action : public Held_Item_Action {
public:

	virtual void Use_Left(Inventory_Item& item, const UseInfo& hit) override;

	virtual void Use_Right(Inventory_Item& item, const UseInfo& hit) override;


private:



};