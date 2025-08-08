#pragma once

#include <memory>

class WorldObject;
class Graphics;

class Renderer : public std::enable_shared_from_this<Renderer>
{
public:
	typedef std::shared_ptr<Renderer> Shared;
	typedef std::weak_ptr<Renderer> Weak;

	virtual void Draw(float dt) = 0;

	void Destroy()
	{
		OnDestroy();
	}

	std::weak_ptr<WorldObject> worldObject() { return m_object; }

	bool Transparent() { return m_transparent; }
	void Transparent(bool value) { m_transparent = value; }

protected:

	void set_worldObject(std::weak_ptr<WorldObject> obj) { m_object = obj; }

	virtual void OnDestroy() = 0;

private:
	std::weak_ptr<WorldObject> m_object;
	bool m_transparent{ false };
};

