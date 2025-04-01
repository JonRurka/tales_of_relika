#pragma once

class WorldObject;
class Graphics;

class Renderer
{
public:
	virtual void Draw(float dt) = 0;

	WorldObject* worldObject() { return m_object; }

	bool Transparent() { return m_transparent; }
	void Transparent(bool value) { m_transparent = value; }

protected:

	void worldObject(WorldObject* obj) { m_object = obj; }

private:
	WorldObject* m_object{ nullptr };
	bool m_transparent{ false };
};

