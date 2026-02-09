#include "Inventory_Item.h"

#include "Item_Type.h"

Inventory_Item Inventory_Item::Empty()
{
	return Inventory_Item(Item_Type::Empty(), 0);
}

Inventory_Item Inventory_Item::From(Item_Type::Shared type)
{
	return Inventory_Item(type, 1);
}


	
