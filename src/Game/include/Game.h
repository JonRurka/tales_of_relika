#pragma once

#include <memory>

#include "game_engine.h"

class Material_Types;

class Game : public Engine
{
public:

	static Game* GameInstance() { return m_instance; }

	static void OpenMainMenu(Scene::SceneStartData data);
	static void OpenVoxelWorld(Scene::SceneStartData data);

protected:

	void Init() override;

	void Update(float dt) override;

private:

	static Game* m_instance;

	Material_Types* m_material_types{nullptr};

	std::weak_ptr<Scene> m_main_menu_scene;
	std::weak_ptr<Scene> m_voxel_world_secen;

	void init_shaders();

	void init_block_types();

	inline static const std::string LOG_LOC{ "GAME" };
};