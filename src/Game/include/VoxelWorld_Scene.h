#pragma once

#include "game_engine.h"
#include "dynamic_compute.h"

class WorldGenController;

class VoxelWorld_Scene : public Scene {
public:



protected:
	void Init() override;

	void Update(float dt) override;

private:

	WorldObject* light_obj_dir{ nullptr };
	Light* light_comp_dir{ nullptr };
	WorldObject* Camera_obj{ nullptr };
	Camera* camera{ nullptr };

	WorldObject* world_gen_controller_obj{ nullptr };
	WorldGenController* world_gen_controller{ nullptr };

	void setup_camera();

	void setup_lights();

	void setup_chunk_gen();


	void create_light_object(WorldObject** obj, Light** light_comp, Light::Light_Type type, glm::vec3 pos, float scale, glm::vec4 color);

	inline static const std::string LOG_LOC{ "VOXELWORLD_SCENE" };
};