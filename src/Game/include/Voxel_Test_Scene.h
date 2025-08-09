#pragma once

#include "game_engine.h"
#include "dynamic_compute.h"

#include <memory>

#include "shared_structures.h"
#include "IVoxelBuilder.h"
#include "SmoothVoxelBuilder.h"

using namespace VoxelEngine;
using namespace DynamicCompute::Compute;

class Standard_Material;
class Opaque_Chunk_Material;

class Voxel_Test_Scene : public Scene {
public:



protected:
	void Init() override;

	void Update(float dt) override;

private:
	Standard_Material::Shared standard_mat;
	Opaque_Chunk_Material::Shared chunk_opaque_mat;
	WorldObject::Weak Camera_obj;
	Camera::Weak camera;

	WorldObject::Weak light_obj_dir;
	Light::Weak light_comp_dir;

	IVoxelBuilder_private::Shared m_builder;


	void create_light_object(WorldObject** obj, Light** light_comp, Light::Light_Type type, glm::vec3 pos, float scale, glm::vec4 color);

	inline static const std::string LOG_LOC{ "VOXEL_TEST_SCENE" };
};
