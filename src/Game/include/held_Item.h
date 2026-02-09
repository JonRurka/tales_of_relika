#pragma once

#include "game_engine.h"

#include "Inventory_Item.h"
#include "ISO_Sampler.h"

#include "Held_Item_Action.h"

#include <unordered_map>

class Held_Item : public Component {

public:
	typedef std::shared_ptr<Held_Item> Shared;
	typedef std::weak_ptr<Held_Item> Weak;

	void Set_Held_Item(Inventory_Item& item);

	void Left_Use(Inventory_Item& item, const Held_Item_Action::UseInfo& info);

	void Right_Use(Inventory_Item& item, const Held_Item_Action::UseInfo& info);

protected:
	void Init() override;

	void Update(float dt) override;

private:

	std::unordered_map<Item_Type::Item_Category, std::unique_ptr<Held_Item_Action>> m_action_processors;

};