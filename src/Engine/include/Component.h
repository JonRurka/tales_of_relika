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

	virtual void Load(json data) = 0;

private:

	void Object(WorldObject* value) { m_object = value; }

	WorldObject* m_object{nullptr};
	std::string m_type_name{"Custom_Component"};

protected:
	virtual void Init() = 0;
	virtual void Update(float dt) = 0;

	void Type_Name(std::string value) { m_type_name = value; }
};

