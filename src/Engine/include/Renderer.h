#pragma once

class WorldObject;
class Graphics;

class Renderer
{
public:
	virtual void Draw(float dt) = 0;

	WorldObject* worldObject() { return m_object; }

protected:

	void worldObject(WorldObject* obj) { m_object = obj; }

private:
	WorldObject* m_object;
};

