#pragma once

#include "game_engine.h"

class Material_Types;

class Game : public Engine
{
public:

	static Game* GameInstance() { return m_instance; }

	static void OpenMainMenu() { m_instance->m_main_menu_scene->Activate(true); }
	static void OpenVoxelWorld(){ m_instance->m_voxel_world_secen->Activate(true); }

protected:

	void Init() override;

	void Update(float dt) override;

private:

	static Game* m_instance;

	Material_Types* m_material_types{nullptr};

	Scene* m_main_menu_scene{ nullptr };
	Scene* m_voxel_world_secen{ nullptr };

	void init_shaders();

	void init_block_types();

	inline static const std::string LOG_LOC{ "GAME" };
};