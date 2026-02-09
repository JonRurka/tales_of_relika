#pragma once

#include "Item_Type.h"

#include <memory>

class Item_Type;

class Inventory_Item {
public:

	static Inventory_Item Empty();

	void Set_Type(Item_Type::Shared val) { m_type = val; }
	Item_Type::Shared Get_Type() { return m_type; }

	void Set_Stack_Size(int val) { m_stack_size = val; }
	int Get_Stack_Size() { return m_stack_size; }

	void Set_Max_Stack_Size(int val) { m_stack_max_size = val; }
	int Get_Max_Stack_Size() { return m_stack_max_size; }

	static Inventory_Item From(Item_Type::Shared type);

	
	/*Inventory_Item(const Inventory_Item& obj) :
		m_type(obj.m_type), 
		m_stack_size(obj.m_stack_size), 
		m_stack_max_size(obj.m_stack_max_size)
	{}*/

	Inventory_Item() :
		m_type(Item_Type::Empty()),
		m_stack_size(0),
		m_stack_max_size(0)
	{ }

private:

	Item_Type::Shared m_type{};
	int m_stack_size{ 0 };
	int m_stack_max_size{ 0 };

	Inventory_Item(Item_Type::Shared type, int stack_size) : 
		m_type{ type }, 
		m_stack_size{ stack_size }
	{ }
	

};