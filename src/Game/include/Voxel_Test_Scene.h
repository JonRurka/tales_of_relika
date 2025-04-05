#pragma once

#include "game_engine.h"
#include "dynamic_compute.h"

#include "shared_structures.h"
#include "IVoxelBuilder.h"
#include "SmoothVoxelBuilder.h"

using namespace VoxelEngine;
using namespace DynamicCompute::Compute;

class Standard_Material;

class Voxel_Test_Scene : public Scene {
public:



protected:
	void Init() override;

	void Update(float dt) override;

private:
	Standard_Material* standard_mat{ nullptr };
	WorldObject* Camera_obj{ nullptr };
	Camera* camera{ nullptr };

	IVoxelBuilder_private* m_builder{ nullptr };


	void create_light_object(WorldObject** obj, Light** light_comp, Light::Light_Type type, glm::vec3 pos, float scale, glm::vec4 color);

	inline static const std::string LOG_LOC{ "VOXEL_TEST_SCENE" };
};
