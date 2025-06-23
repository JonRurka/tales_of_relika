#pragma once

#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class WorldObject;

class Component
{
	friend class WorldObject;
public:

	WorldObject* Object() { return m_object; }

	std::string Type_Name() { return m_type_name; }

	//virtual void Load(json data) = 0;

	static Component* Load_Component(WorldObject* obj, json data);

	void Destroy();

	virtual void Load(json data) {};

private:

	void Object(WorldObject* value) { m_object = value; }

	void Component_Index(int idx) { m_comp_idx = idx; }

	WorldObject* m_object{nullptr};
	int m_comp_idx{ 0 };
	std::string m_type_name{"Custom_Component"};

protected:
	virtual void Init() = 0;
	virtual void Update(float dt) = 0;
	virtual void OnDestroy() {};

	WorldObject* Instantiate();

	WorldObject* Instantiate(std::string name);

	void Type_Name(std::string value) { m_type_name = value; }
};

