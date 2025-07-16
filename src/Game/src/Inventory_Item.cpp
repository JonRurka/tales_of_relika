#include "Inventory_Item.h"

#include "Item_Type.h"

Inventory_Item Inventory_Item::Empty()
{
	return Inventory_Item();
}

Inventory_Item::Inventory_Item(Item_Type* type, int stack_size)
	: m_type{type}, m_stack_size{stack_size}
{
}

Inventory_Item::Inventory_Item() 
{
	m_type = Item_Type::Empty();
	m_stack_size = 0;
}