#include "held_Item.h"

#include "Held_Terrain_Mat_Action.h"
#include "Held_Structure_Mat_Action.h"
#include "Held_Empty_Action.h"

void Held_Item::Set_Held_Item(Inventory_Item& item)
{
	assert(m_action_processors.contains(item.Get_Type()->Get_Category()));

	Held_Item_Action& actions = *m_action_processors[item.Get_Type()->Get_Category()];
	const Held_Item_Action::ItemWorldObject& obj = actions.Hold_Object(item);

	if (obj.Preset == Held_Item_Action::Mesh_Presets::None)
		return;

	// TODO: Load model of item
}

void Held_Item::Left_Use(Inventory_Item& item, const Held_Item_Action::UseInfo& info)
{
	assert(m_action_processors.contains(item.Get_Type()->Get_Category()));
	Held_Item_Action& actions = *m_action_processors[item.Get_Type()->Get_Category()];
	actions.Use_Left(item, info);
}

void Held_Item::Right_Use(Inventory_Item& item, const Held_Item_Action::UseInfo& info)
{
	assert(m_action_processors.contains(item.Get_Type()->Get_Category()));
	Held_Item_Action& actions = *m_action_processors[item.Get_Type()->Get_Category()];
	actions.Use_Right(item, info);
}

void Held_Item::Init()
{
	m_action_processors[Item_Type::Item_Category::None] = std::make_unique<Held_Empty_Action>();// Held_Empty_Action();
	m_action_processors[Item_Type::Item_Category::Terrain_Material] = std::make_unique <Held_Terrain_Mat_Action>();
	m_action_processors[Item_Type::Item_Category::Structural_Material] = std::make_unique<Held_Structure_Mat_Action>();

}

void Held_Item::Update(float dt)
{
}
