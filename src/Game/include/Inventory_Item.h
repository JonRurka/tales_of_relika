#pragma once

class Item_Type;

class Inventory_Item {
public:

	static Inventory_Item Empty();

	Item_Type* Get_Type() { return m_type; }

	int Get_Stack_Size() { return m_stack_size; }

	void Set_Type(Item_Type* val) { m_type = val; }

	void Set_Stack_Size(int val) { m_stack_size = val; }

	Inventory_Item(Item_Type* type, int stack_size);
	Inventory_Item();

private:

	Item_Type* m_type{nullptr};
	int m_stack_size{ 0 };

	

};