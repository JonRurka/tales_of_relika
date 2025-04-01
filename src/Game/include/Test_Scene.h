#pragma once

#include "game_engine.h"
#include "dynamic_compute.h"

using namespace DynamicCompute::Compute;

class Standard_Material;

class Test_Scene  : public Scene{
public:


protected:
	void Init() override;

	void Update(float dt) override;

private:

	Standard_Material* standard_mat{nullptr};
	Standard_Material* floor_mat{ nullptr };
	WorldObject* Camera_obj{ nullptr };
	Camera* camera{ nullptr };

	float m_timer{ 0 };

	Vulkan_Device_Info m_device{};
	ComputeInterface::ControllerInfo m_controllerInfo{};
	IComputeController* m_controller{ nullptr };
	IComputeProgram* m_program{ nullptr };

	IComputeBuffer* in_vertices{ nullptr };
	IComputeBuffer* in_normals{ nullptr };
	IComputeBuffer* in_colors{ nullptr };
	IComputeBuffer* in_texcoords{ nullptr };

	IComputeBuffer* out_vbo{ nullptr };

	void create_light_object(WorldObject** obj, Light** light_comp, Light::Light_Type type, glm::vec3 pos, float scale, glm::vec4 color);

	void Setup_gpu_program(int size);

	const std::string LOG_LOC{ "TEST_SCENE" };
};
