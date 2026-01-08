#pragma once

#include <memory>

#include "Camera.h"

class WorldObject;
class Graphics;
class Camera;

class Renderer : public std::enable_shared_from_this<Renderer>
{
public:
	typedef std::shared_ptr<Renderer> Shared;
	typedef std::weak_ptr<Renderer> Weak;

	virtual bool Draw(std::weak_ptr<Camera> cam, float dt) = 0;

	void Destroy()
	{
		OnDestroy();
	}

	std::weak_ptr<WorldObject> worldObject() { return m_object; }

	bool Transparent() const { return m_transparent; }
	void Transparent(bool value) { m_transparent = value; }

	bool Active() const { return m_active; }
	void Active(bool active) { m_active = active; }

protected:

	void set_worldObject(std::weak_ptr<WorldObject> obj) { m_object = obj; }

	virtual void OnDestroy() = 0;

private:
	std::weak_ptr<WorldObject> m_object;
	bool m_transparent{ false };
	bool m_active{ true };
};

