#pragma once

#include <string>
#include <memory>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class WorldObject;

class Component : public std::enable_shared_from_this<Component>
{
	friend class WorldObject;
public:
	typedef std::shared_ptr<Component> Shared;
	typedef std::weak_ptr<Component> Weak;

	WorldObject& Object();
	std::weak_ptr<WorldObject> Object_Ptr() { return m_object; }

	std::string Type_Name() { return m_type_name; }

	//virtual void Load(json data) = 0;

	static Component* Load_Component(WorldObject* obj, json data);

	void Enabled(bool val) { m_enabled = val; }
	bool Enabled() { return m_enabled; }

	void Destroy();

	virtual void Load(json data) {};

private:

	void Object(std::weak_ptr<WorldObject> value) { m_object = value; }

	void Component_Index(int idx) { m_comp_idx = idx; }

	std::weak_ptr<WorldObject> m_object;
	int m_comp_idx{ 0 };
	std::string m_type_name{"Custom_Component"};
	bool m_enabled{ true };
	

protected:
	virtual void Init() = 0;
	virtual void Update(float dt) = 0;
	virtual void FixedUpdate(float dt) {};
	virtual void OnDestroy() {};

	std::weak_ptr<WorldObject> Instantiate();

	std::weak_ptr<WorldObject> Instantiate(std::string name);

	void Type_Name(std::string value) { m_type_name = value; }
};

